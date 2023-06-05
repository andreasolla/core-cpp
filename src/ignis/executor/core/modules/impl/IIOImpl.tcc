
#include "IIOImpl.h"
#include "ignis/executor/core/io/IJsonReader.h"
#include "ignis/executor/core/io/IJsonWriter.h"
#include "ignis/executor/core/io/IPrinter.h"
#include "ignis/executor/core/protocol/IObjectProtocol.h"
#include "ignis/executor/core/transport/IZlibTransport.h"
#include <climits>
#include <fstream>
#include "libhdfsExplorer.h"
#include <iostream>
#include <streambuf>
#include <cstring>
#include <ostream> 
#include <string.h>
#include <vector>

#define IIOImplClass ignis::executor::core::modules::impl::IIOImpl

class HDFStreamBuffer : public std::streambuf
{
public:
    static constexpr std::streamsize BUF_SIZE = 4096; // 4 KB

    HDFStreamBuffer()
    {
        file = nullptr;
        hdfs = nullptr;
    }

    ~HDFStreamBuffer()
    {
        sync(); // Ensure that the buffer is flushed before we destroy the object
        delete[] file;
    }

    void setFile(char* f)
    {
        int length = strlen(f);
        file = new char[length + 1];
        strcpy(file, f);
    }

protected:
    virtual std::streamsize xsputn(const char_type* s, std::streamsize n)
    {
        const char* from = s;
        std::streamsize remaining = n;

        while (remaining > 0) {
            std::streamsize available = BUF_SIZE - buffer.size();

            if (available > 0) {
                std::streamsize to_copy = (available < remaining) ? available : remaining;
                buffer.insert(buffer.end(), from, from + to_copy);

                remaining -= to_copy;
                from += to_copy;
            }

            if (buffer.size() == BUF_SIZE) {
                if (!flushBuffer()) {
                    return 0;
                }
            }
        }

        return n - remaining;
    }

    virtual int_type overflow(int_type ch)
    {
        if (ch == traits_type::eof()) {
            return traits_type::eof();
        }    

        buffer.push_back(static_cast<char>(ch));

        if (buffer.size() == BUF_SIZE) {
            if (!flushBuffer()) {
                return traits_type::eof();
            }
        }

        return ch;
    }

    virtual int sync()
    {
        return flushBuffer() ? 0 : -1;
    }

private:
    bool flushBuffer()
    {
        if (buffer.empty()) {
            return true;
        }
        int f = Open(file, 'w');

        if (f < 0) {
            return false;
        }

        // Convert buffer to a null-terminated string.
        buffer.push_back('\0');
        Write(f, &buffer[0]);

        Close(f, 'w');

        buffer.clear();

        return true;
    }

    char* file;
    std::vector<char> buffer;
};

template<typename Tp>
int64_t IIOImplClass::partitionApproxSize() {
    IGNIS_TRY()
    IGNIS_LOG(info) << "IO: calculating partition size";
    auto group = executor_data->getPartitions<Tp>();
    if (group->partitions() == 0) { return 0; }

    int64_t size = 0;
    if (executor_data->getPartitionTools().isMemory(*group)) {
        for (auto &partition : *group) { size += partition->size(); }

        if (executor_data->mpi().isContiguousType<Tp>()) {
            size *= sizeof(Tp);
        } else {
            size *= sizeof(Tp) + executor_data->getProperties().transportElemSize();
        }
    } else {
        for (auto &partition : *group) { size += partition->bytes(); }
    }

    return size;
    IGNIS_CATCH()
}

template<typename Tp>
void IIOImplClass::partitionObjectFile(const std::string &path, int64_t first, int64_t partitions) {
    IGNIS_TRY()
    IGNIS_LOG(info) << "IO: reading partitions object file";
    auto group = executor_data->getPartitionTools().newPartitionGroup<Tp>(partitions);

    IGNIS_OMP_EXCEPTION_INIT()
#pragma omp parallel num_threads(ioCores())
    {
        IGNIS_OMP_TRY()
#pragma omp for schedule(dynamic)
        for (int64_t p = 0; p < partitions; p++) {
            std::string file_name = partitionFileName(path, first + p);
            openFileRead(file_name);//Only to check
            storage::IDiskPartition<Tp> open(file_name, 0, true, true);
            (*group)[p]->copyFrom(open);
            (*group)[p]->fit();
        }
        IGNIS_OMP_CATCH()
    }
    IGNIS_OMP_EXCEPTION_END()
    executor_data->setPartitions(group);
    IGNIS_CATCH()
}

template<typename Tp>
void IIOImplClass::partitionJsonFile(const std::string &path, int64_t first, int64_t partitions) {
    IGNIS_TRY()
    IGNIS_LOG(info) << "IO: reading partitions json file";
    auto group = executor_data->getPartitionTools().newPartitionGroup<Tp>(partitions);

    IGNIS_OMP_EXCEPTION_INIT()
#pragma omp parallel num_threads(ioCores())
    {
        IGNIS_OMP_TRY()
#pragma omp for schedule(dynamic)
        for (int64_t p = 0; p < partitions; p++) {
            auto file_name = partitionFileName(path, first + p)  + ".json";
            std::ifstream file = openFileRead(file_name);
            io::IJsonReader<api::IWriteIterator<Tp>> reader;
            auto write_iterator = (*group)[p]->writeIterator();
            reader(file, *write_iterator);
            (*group)[p]->fit();
        }
        IGNIS_OMP_CATCH()
    }
    IGNIS_OMP_EXCEPTION_END()
    executor_data->setPartitions(group);
    IGNIS_CATCH()
}

template<typename Tp>
void IIOImplClass::saveAsObjectFile(const std::string &path, int8_t compression, int64_t first) {
    IGNIS_TRY()
    IGNIS_LOG(info) << "IO: saving as object file";
    auto group = executor_data->getAndDeletePartitions<Tp>();
    IGNIS_OMP_EXCEPTION_INIT()
#pragma omp parallel num_threads(ioCores())
    {
        IGNIS_OMP_TRY()
#pragma omp for schedule(dynamic)
        for (int64_t p = 0; p < group->partitions(); p++) {
            std::string file_name;
#pragma omp critical
            {
                file_name = partitionFileName(path, first + p);
                openFileWrite(file_name);//Only to check
            };

            storage::IDiskPartition<Tp> save(file_name, compression, true);
            (*group)[p]->copyTo(save);
            save.sync();
            (*group)[p].reset();
        }
        IGNIS_OMP_CATCH()
    }
    IGNIS_OMP_EXCEPTION_END()
    IGNIS_CATCH()
}

template<typename Tp>
void IIOImplClass::saveAsTextFile(const std::string &path, int64_t first) {
    IGNIS_TRY()
    IGNIS_LOG(info) << "IO: saving as text file";
    auto group = executor_data->getAndDeletePartitions<Tp>();
    bool isMemory = executor_data->getPartitionTools().isMemory(*group);

    IGNIS_OMP_EXCEPTION_INIT()
#pragma omp parallel num_threads(ioCores())
    {
        IGNIS_OMP_TRY()
#pragma omp for schedule(dynamic)
        for (int64_t p = 0; p < group->partitions(); p++) {
            std::ofstream file;
#pragma omp critical
            {
                auto file_name = partitionFileName(path, first + p);
                file = openFileWrite(file_name);
            };

            auto &part = *(*group)[p];
            if (isMemory) {
                auto &men_part = executor_data->getPartitionTools().toMemory(part);
                io::IPrinter<typename std::remove_reference<decltype(men_part.inner())>::type> printer;
                printer(file, men_part.inner());
            } else {
                io::IPrinter<api::IReadIterator<Tp>> printer;
                printer(file, *part.readIterator());
            }
            (*group)[p].reset();
        }
        IGNIS_OMP_CATCH()
    }
    IGNIS_OMP_EXCEPTION_END()
    IGNIS_CATCH()
}

template<typename Tp>
void IIOImplClass::saveAsHdfsFile(const std::string &path, int64_t first) {
    IGNIS_TRY()
    IGNIS_LOG(info) << "IO: saving as hdfs file";
    auto group = executor_data->getAndDeletePartitions<Tp>();
    bool isMemory = executor_data->getPartitionTools().isMemory(*group);

    std::string aux = "/" + path.substr(path.find('/') + 2);

    std::string hdfsHost = executor_data->getProperties().hdfsPath();
    int length = hdfsHost.length();
    char *chdfsHost = new char[length + 1];
    strcpy(chdfsHost, hdfsHost.c_str());
    if (NewHdfsClient(chdfsHost) < 0) {
        IGNIS_LOG(error) << "IO: hdfs client not connected";
        return;
    }
    IGNIS_LOG(info) << "IO: hdfs client connected";
    
    IGNIS_OMP_EXCEPTION_INIT()
#pragma omp parallel num_threads(ioCores())
    {
        IGNIS_OMP_TRY()

        #pragma omp barrier
#pragma omp for schedule(dynamic)
        for (int64_t p = 0; p < group->partitions(); p++) {
            HDFStreamBuffer stream_file;
            std::ostream output_stream(&stream_file);
#pragma omp critical
            {
                auto file_name = partitionFileName(aux, first + p);
                int length = file_name.length();
                char *fpath = new char[length + 1];
                strcpy(fpath, file_name.c_str());
                stream_file.setFile(fpath);
                delete[] fpath;
            };

            auto &part = *(*group)[p];
            if (isMemory) {
                auto &men_part = executor_data->getPartitionTools().toMemory(part);
                io::IPrinter<typename std::remove_reference<decltype(men_part.inner())>::type> printer;
                printer(output_stream, men_part.inner());
            } else {
                io::IPrinter<api::IReadIterator<Tp>> printer;
                printer(output_stream, *part.readIterator());
            }
            (*group)[p].reset();
            
        }
        IGNIS_OMP_CATCH()
    }
    IGNIS_OMP_EXCEPTION_END()
    CloseConnection();
    delete[] chdfsHost;
    IGNIS_CATCH()
}

template<typename Tp>
void IIOImplClass::saveAsJsonFile(const std::string &path, int64_t first, bool pretty) {
    IGNIS_TRY()
    IGNIS_LOG(info) << "IO: saving as json file";
    auto group = executor_data->getAndDeletePartitions<Tp>();

    IGNIS_OMP_EXCEPTION_INIT()
#pragma omp parallel num_threads(ioCores())
    {
        IGNIS_OMP_TRY()
#pragma omp for schedule(dynamic)
        for (int64_t p = 0; p < group->partitions(); p++) {
            std::ofstream file;
#pragma omp critical
            {
                auto file_name = partitionFileName(path, first + p) + ".json";
                file = openFileWrite(file_name);
            };
            auto &part = *(*group)[p];
            io::IJsonWriter<api::IReadIterator<Tp>> writer;
            writer(file, *part.readIterator(), pretty);
            (*group)[p].reset();
        }
        IGNIS_OMP_CATCH()
    }
    IGNIS_OMP_EXCEPTION_END()

    IGNIS_CATCH()
}

#undef IIOImplClass
