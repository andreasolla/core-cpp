
#include "IIOImpl.h"
#include "ignis/executor/api/IJsonValue.h"
#include "ignis/executor/core/storage/IVoidPartition.h"
#include "ignis/executor/core/exception/IException.h"
#include <algorithm>
#include <fstream>
#include <ghc/filesystem.hpp>
#include <rapidjson/error/en.h>
#include <rapidjson/istreamwrapper.h>
#include <rapidjson/reader.h>
#include <map>
#include <chrono>

using namespace ignis::executor::core::modules::impl;
using namespace ignis::executor::core::storage;
using ignis::executor::api::IJsonValue;

IIOImpl::IIOImpl(std::shared_ptr<IExecutorData> &executorData) : IBaseImpl(executorData) {}

IIOImpl::~IIOImpl() {}

class Block {
public:
    int BlockID;
    int NumBytes;
    std::string IpAddr;

    Block() : BlockID(0), NumBytes(0), IpAddr("") {}

    Block(int blockID, int numBytes, const std::string& ipAddr)
        : BlockID(blockID), NumBytes(numBytes), IpAddr(ipAddr) {}
};

std::string IIOImpl::partitionFileName(const std::string &path, int64_t index) {
    if (!ghc::filesystem::is_directory(path)) {
        std::error_code error;
        ghc::filesystem::create_directories(path, error);
        if (!ghc::filesystem::is_directory(path)) {
            throw exception::IInvalidArgument("Unable to create directory " + path + " " + error.message());
        }
    }

    auto str_index = std::to_string(index);
    auto zeros = std::max(6 - (int) str_index.size(), 0);
    return path + "/part" + std::string(zeros, '0') + str_index;
}

std::ifstream IIOImpl::openFileRead(const std::string &path) {
    IGNIS_LOG(info) << "IO: opening file " << path;
    if (!ghc::filesystem::exists(path)) { throw exception::IInvalidArgument(path + " was not found"); }
    if (!ghc::filesystem::is_regular_file(path)) { throw exception::IInvalidArgument(path + " was not a file"); }

    std::ifstream file(path, std::ifstream::in | std::ifstream::binary);
    if (!file.good()) { throw exception::IInvalidArgument(path + " cannot be opened"); }
    IGNIS_LOG(info) << "IO: file opening successful";
    return file;
}

std::ofstream IIOImpl::openFileWrite(const std::string &path) {
    IGNIS_LOG(info) << "IO: creating file " << path;
    if (ghc::filesystem::exists(path)) {
        if (executor_data->getProperties().ioOverwrite()) {
            IGNIS_LOG(warning) << "IO: " << path << " already exists";
            if (!ghc::filesystem::remove(path)) { throw exception::ILogicError(path + " can not be removed"); }
        } else {
            throw exception::IInvalidArgument(path + " already exists");
        }
    }
    std::ofstream file(path, std::ifstream::out | std::ifstream::binary | std::fstream::trunc);
    if (!file.good()) { throw exception::IInvalidArgument(path + " cannot be opened"); }
    IGNIS_LOG(info) << "IO: file created successful";
    return file;
}

void getline(std::ifstream &file, std::string &str, std::string &buffer, const std::string &delim,
             const std::vector<std::string> &exs = {}) {
    if (delim.length() == 1) {
        std::getline(file, str, delim[0]);
        return;
    }
    int dsize = delim.size();
    int64_t overlap = 0;
    str.clear();
    while (std::getline(file, buffer, delim.back())) {
        str.append(buffer);
        str += delim.back();
        if (str.size() >= dsize && str.rfind(delim) == (str.size() - dsize)) {
            if (!exs.empty()) {
                if (str.size() - overlap < dsize) { continue; }
                bool f = false;
                for (auto &ex : exs) {
                    if (str.size() >= ex.size() && str.find(ex) == (str.size() - ex.size())) {
                        overlap = str.size();
                        f = true;
                        break;
                    }
                }
                if (f) { continue; }
            }
            str.resize(str.size() - dsize);
            return;
        }
    }
}

void IIOImpl::plainFile(const std::string &path, int64_t minPartitions, const std::string &delim) {
    IGNIS_TRY()
    IGNIS_LOG(info) << (delim == "\n" ? "IO: reading text file" : "IO: reading plain file");
    auto size = ghc::filesystem::file_size(path);
    IGNIS_LOG(info) << "IO: file has " << size << " Bytes";
    auto result = executor_data->getPartitionTools().newPartitionGroup<std::string>();
    auto io_cores = ioCores();
    decltype(result) thread_groups[io_cores];
    size_t total_bytes = 0;
    size_t elements = 0;

    IGNIS_OMP_EXCEPTION_INIT()
#pragma omp parallel reduction(+ : total_bytes, elements) firstprivate(minPartitions) private(start, end, diff) num_threads(io_cores)
    {
        IGNIS_OMP_TRY()
        std::ifstream file = openFileRead(path);
        auto id = executor_data->getContext().threadId();
        auto globalThreadId = executor_data->getContext().executorId() * io_cores + id;
        auto threads = executor_data->getContext().executors() * io_cores;
        size_t ex_chunk = size / threads;
        size_t ex_chunk_init = globalThreadId * ex_chunk;
        size_t ex_chunk_end = ex_chunk_init + ex_chunk;
        size_t minPartitionSize = executor_data->getProperties().partitionMinimal();
        minPartitions = (int64_t) std::ceil(minPartitions / (float) threads);
        std::string str, buffer;
        std::vector<std::string> exs;
        std::string ldelim = delim;
        int esize = 0;
        if (ldelim.find('!') != std::string::npos) {
            std::string flag = "\1";
            while (ldelim.find(flag) != std::string::npos) { flag += "\1"; }
            auto replaceAll = [](std::string &subject, const std::string &search, const std::string &replace) {
                size_t pos = 0;
                while ((pos = subject.find(search, pos)) != std::string::npos) {
                    subject.replace(pos, search.length(), replace);
                    pos += replace.length();
                }
            };
            replaceAll(ldelim, "\\!", flag);
            std::stringstream fields(ldelim);
            std::string field;
            for (int i = 0; std::getline(fields, field, '!'); i++) {
                replaceAll(field, flag, "!");
                if (i == 0) {
                    ldelim = field;
                } else {
                    exs.push_back(field + ldelim);
                }
            }
        }
        for (auto &ex : exs) {
            if (ex.size() > esize) { esize = ex.size(); }
        }
        if (ldelim.empty()) { ldelim = "\n"; }
        int dsize = ldelim.size();
        if (globalThreadId > 0) {
            size_t padding = ex_chunk_init >= (dsize + esize) ? ex_chunk_init - (dsize + esize) : 0;
            file.seekg(padding);
            int value;
            if (dsize == 1 && esize == 0) {
                do { padding++; } while ((value = file.get()) != ldelim[0] && value != EOF);
            } else {
                do {
                    getline(file, str, buffer, ldelim, exs);
                    padding += str.size() + dsize;
                } while (ex_chunk_init > padding);
            }
            ex_chunk_init = padding;
            if (globalThreadId == threads - 1) { ex_chunk_end = size; }
        }

        if (ex_chunk / minPartitionSize < minPartitions) { minPartitionSize = ex_chunk / minPartitions; }

        thread_groups[id] = executor_data->getPartitionTools().newPartitionGroup<std::string>();
        auto partition = executor_data->getPartitionTools().newPartition<std::string>();
        auto write_iterator = partition->writeIterator();
        thread_groups[id]->add(partition);
        size_t partitionInit = ex_chunk_init;
        size_t filepos = ex_chunk_init;
        
        if (executor_data->getPartitionTools().isMemory(*partition)) {
            auto part_men = executor_data->getPartitionTools().toMemory(partition);
            while (filepos < ex_chunk_end) {
                if ((filepos - partitionInit) > minPartitionSize) {
                    part_men->fit();
                    part_men = executor_data->getPartitionTools().newMemoryPartition<std::string>();
                    thread_groups[id]->add(part_men);
                    partitionInit = filepos;
                }
                getline(file, str, buffer, ldelim, exs);
                filepos += str.size() + dsize;
                elements++;
                part_men->inner().push_back(str);
            }
        } else {
            while (filepos < ex_chunk_end) {
                if ((filepos - partitionInit) > minPartitionSize) {
                    partition->fit();
                    partition = executor_data->getPartitionTools().newPartition<std::string>();
                    write_iterator = partition->writeIterator();
                    thread_groups[id]->add(partition);
                    partitionInit = filepos;
                }
                getline(file, str, buffer, ldelim, exs);
                filepos += str.size() + dsize;
                elements++;
                write_iterator->write(str);
            }
        }

        total_bytes += (size_t) file.tellg() - ex_chunk_init;

        IGNIS_OMP_CATCH()
    }
    IGNIS_OMP_EXCEPTION_END()
    start = std::chrono::steady_clock::now();
    for (auto group : thread_groups) {
        for (auto part : *group) { result->add(part); }
    }

    IGNIS_LOG(info) << "IO: created  " << result->partitions() << " partitions, " << elements << " lines and "
                    << total_bytes << " Bytes read ";

    executor_data->setPartitions(result);
    IGNIS_CATCH()
}

std::vector<Block> GetBlocks(int file) {
    int size;
    BlockInfo* Cblocks = GetBlockInfo(file, &size);

    std::vector<Block> blocks;
    for (int i = 0; i < size; i++) {
        Block block(Cblocks[i].blockid, Cblocks[i].numbytes, Cblocks[i].ipaddr);
        blocks.push_back(block);
    }

    FreeBlockInfo(Cblocks, size);

    return blocks;
}

std::vector<int> assignedBlocks(const std::vector<Block> &blocks, std::shared_ptr<ignis::executor::core::IExecutorData> executor_data) {
    IGNIS_LOG(info) << "IO: distributing blocks";
    int executorId = executor_data->getContext().executorId();
    std::string executorIP = executor_data->getProperties().host();
    int executors = executor_data->getContext().executors();
    int blocksPerExecutor = ceil(blocks.size() / executors);

    std::vector<Block> executorBlocks;
    std::map<int, Block> notAssignedBlocks;
    for (int i = 0; i < blocks.size(); i++) {
        notAssignedBlocks[blocks[i].BlockID] = blocks[i];
        if (blocks[i].IpAddr == executorIP) {
            executorBlocks.push_back(blocks[i]);
        }
    }
    
    // Comunicacion con los demas ejecutores para repartir los bloques
    std::vector<std::pair<const int, std::vector<Block>>> myBlocks;
    myBlocks.push_back(std::make_pair(executorId, executorBlocks));

    //------------------------------
    // Obtengo el orden de los ejecutores
    int executorsOrder[executors];
    MPI_Allgather(&executorId, 1, MPI::INT, executorsOrder, 1, MPI::INT, executor_data->getContext().mpiGroup());
    
    //Obtengo el numero de bloques de cada ejecutor
    int executorsNBlocks[executors];
    int numBlocks = executorBlocks.size();
    MPI_Allgather(&numBlocks, 1, MPI::INT, executorsNBlocks, 1, MPI::INT, executor_data->getContext().mpiGroup());
    
    // Paso los BlockID de executorBlocks a un array de bloques
    int executorBlocksArray[executorBlocks.size()];
    for (int i = 0; i < executorBlocks.size(); i++) {
        executorBlocksArray[i] = executorBlocks[i].BlockID;
    }
    
    // Array de posiciones donde se guardaran los bloques de cada ejecutor
    int executorsBlocksPos[executors];
    executorsBlocksPos[0] = 0;
    for (int i = 1; i < executors; i++) {
        executorsBlocksPos[i] = executorsBlocksPos[i-1] + executorsNBlocks[i-1];
    }
    
    //Obtengo el numero total de bloques
    int totalBlocks = 0;
    for (int i = 0; i < executors; i++) {
        totalBlocks += executorsNBlocks[i];
    }
    // Obtengo todos los bloques de todos los ejecutores con allgatherv
    int allBlocksArray[totalBlocks];
    MPI_Allgatherv(executorBlocksArray, executorBlocks.size(), MPI::INT, allBlocksArray, executorsNBlocks, executorsBlocksPos, MPI::INT, executor_data->getContext().mpiGroup());
    
    std::vector<std::pair<int, std::vector<int>>> allBlocks;

    for (int i = 0; i < executors; i++) {
        std::vector<int> blocks;
        for (int j = 0; j < executorsNBlocks[i]; j++) {
            blocks.push_back(allBlocksArray[executorsBlocksPos[i] + j]);
        }
        std::pair<int, std::vector<int>> pair = std::make_pair(executorsOrder[i], blocks);
        allBlocks.push_back(pair);
    }
    
    std::sort(allBlocks.begin(), allBlocks.end(), [](const std::pair<int, std::vector<int>>& a, const std::pair<int, std::vector<int>>& b) {
        return a.first < b.first; // Ordenar por el primer elemento del par
    });
    
    // Reparto de bloques por IP
    std::vector<int> takenBlocks;
    std::vector<int> assignedBlocks;
    std::vector<int> numberBlocks(executors, 0);
    for (int i = 0; i < executors; i++) {
        int taken = 0;
        int pos = 0;
        int nBlocks = 0;
        while (allBlocks[i].second.size() > 0 && taken < blocksPerExecutor && pos < allBlocks[i].second.size()) {
            // si no esta en takenBlocks, se asigna
            if (std::find(takenBlocks.begin(), takenBlocks.end(), allBlocks[i].second[pos]) != takenBlocks.end()) {
                takenBlocks.push_back(allBlocks[i].second[pos]);
                taken += 1;
                nBlocks += 1;
                if (allBlocks[i].first == executorId) {
                    assignedBlocks.push_back(allBlocks[i].second[pos]);
                }
            }
            pos += 1;
        }
        numberBlocks[i] = nBlocks;
    }
    
    // Bloques que no han sido asignados
    std::vector<int> lostBlocks;
    for (int i = 0; i < blocks.size(); i++) {
        if (std::find(takenBlocks.begin(), takenBlocks.end(), blocks[i].BlockID) == takenBlocks.end()) { lostBlocks.push_back(blocks[i].BlockID); }
    }

    // Si aun no se han asignado todos los bloques, se asignan los que faltan
    if (assignedBlocks.size() < blocksPerExecutor && lostBlocks.size() > 0) {
        int n_executor = 0;
        int i = 0;
        
        while (n_executor < executorId && lostBlocks.size() > i) {
            i += blocksPerExecutor - numberBlocks[n_executor];
            n_executor += 1;
        }
        while (assignedBlocks.size() < blocksPerExecutor && lostBlocks.size() > i) {
            assignedBlocks.push_back(lostBlocks[i]);
            i += 1;
        }
    }

    return assignedBlocks;
}

void hdfsNotOrdering(const std::string &path, int64_t minPartitions,
                     std::shared_ptr<ignis::executor::core::IExecutorData> executor_data, int io_cores) {
    std::string aux = "/" + path.substr(path.find('/') + 2);
    int length = aux.length();
    char *fpath = new char[length + 1];
    strcpy(fpath, aux.c_str());

    std::string hdfsHost = executor_data->getProperties().hdfsPath();
    int length2 = hdfsHost.length();
    char *chdfsHost = new char[length2 + 1];
    strcpy(chdfsHost, hdfsHost.c_str());
    if (NewHdfsClient(chdfsHost) < 0) {
        throw ignis::executor::core::exception::IException("IO: hdfs client not connected");
        return;
    }
    IGNIS_LOG(info) << "IO: hdfs client connected";
    delete[] chdfsHost;

    auto size = Size(fpath);
    if (size < 0) {
        throw ignis::executor::core::exception::IException("IO: problem opening hdfs file");
        return;
    }
    IGNIS_LOG(info) << "IO: file has " << size << " Bytes";
    auto result = executor_data->getPartitionTools().newPartitionGroup<std::string>();
    decltype(result) thread_groups[io_cores];
    size_t total_bytes = 0;
    size_t elements = 0;
    std::string host = executor_data->getProperties().host();

    int file = Open(fpath, 'r');
    const std::vector<Block> blocks = GetBlocks(file);
    auto blocksToRead = assignedBlocks(blocks, executor_data);
    
    int first = blocks[0].BlockID;
    int blockSize = blocks[0].NumBytes;
    std::vector<Block> myBlocks;

    for (int i = 0; i < blocks.size(); i++) {
        if (std::find(blocksToRead.begin(), blocksToRead.end(), blocks[i].BlockID) != blocksToRead.end()) {
            myBlocks.push_back(blocks[i]);
        }
    }

    Close(file, 'r');
    
    IGNIS_OMP_EXCEPTION_INIT()
#pragma omp parallel reduction(+ : total_bytes, elements) firstprivate(minPartitions, myBlocks, blockSize) num_threads(io_cores)
    {
        IGNIS_OMP_TRY()
        auto file = Open(fpath, 'r');
        auto id = executor_data->getContext().threadId();
        auto globalThreadId = executor_data->getContext().executorId() * io_cores + id;
        auto threads = executor_data->getContext().executors() * io_cores;
        size_t minPartitionSize = blockSize;
        std::string str;
        std::string ldelim = "\n";
        int esize = 0;

        for (int i=0; i < myBlocks.size(); i++) {
            size_t ex_chunk_init = (myBlocks[i].BlockID - first) * blockSize;
            size_t ex_chunk_end = ex_chunk_init + myBlocks[i].NumBytes;
            size_t filepos = ex_chunk_init;

            thread_groups[id] = executor_data->getPartitionTools().newPartitionGroup<std::string>();
            auto partition = executor_data->getPartitionTools().newPartition<std::string>();
            auto write_iterator = partition->writeIterator();
            thread_groups[id]->add(partition);
            size_t partitionInit = ex_chunk_init;

            if (myBlocks[i].BlockID != first) {
                size_t padding = ex_chunk_init >= (ldelim.size()) ? ex_chunk_init - (ldelim.size()) : 0;
                Seek(file, padding, 0);
                while (ex_chunk_init > padding) { padding += std::string((ReadLine(file))).size(); }

                ex_chunk_init = padding;
                if (globalThreadId == threads - 1) { ex_chunk_end = size; }
            }

            if (executor_data->getPartitionTools().isMemory(*partition)) {
                auto part_men = executor_data->getPartitionTools().toMemory(partition);
                while (filepos < ex_chunk_end) {
                    if ((filepos - partitionInit) > minPartitionSize) {
                        part_men->fit();
                        part_men = executor_data->getPartitionTools().newMemoryPartition<std::string>();
                        thread_groups[id]->add(part_men);
                        partitionInit = filepos;
                    }
                    
                    str = ReadLine(file);
                    filepos += str.size();
                    elements++;
                    str.resize(str.size() - ldelim.size());
                    part_men->inner().push_back(str);
                }
            } else {
                while (filepos < ex_chunk_end) {
                    if ((filepos - partitionInit) > minPartitionSize) {
                        partition->fit();
                        partition = executor_data->getPartitionTools().newPartition<std::string>();
                        write_iterator = partition->writeIterator();
                        thread_groups[id]->add(partition);
                        partitionInit = filepos;
                    }
                    
                    str = ReadLine(file);
                    filepos += str.size();
                    elements++;
                    str.resize(str.size() - ldelim.size());
                    write_iterator->write(str);
                }
            }
            total_bytes += (size_t) filepos - ex_chunk_init;
        }

        Close(file, 'r');
        
        IGNIS_OMP_CATCH()
    }
    IGNIS_OMP_EXCEPTION_END()

    delete[] fpath;

    for (auto group : thread_groups) {
        for (auto part : *group) { result->add(part); }
    }

    IGNIS_LOG(info) << "IO: created  " << result->partitions() << " partitions, " << elements << " lines and "
                    << total_bytes << " Bytes read ";

    executor_data->setPartitions(result);
    
    CloseConnection();
}

void hdfsTextFile(const std::string &path, int64_t minPartitions,
                  std::shared_ptr<ignis::executor::core::IExecutorData> executor_data, int io_cores) {
    IGNIS_TRY()
    IGNIS_LOG(info) << "IO: reading hdfs file";

    if (!executor_data->getProperties().hdfsPreserveOrder()) {
        return hdfsNotOrdering(path, minPartitions, executor_data, io_cores);
    }

    std::string aux = "/" + path.substr(path.find('/') + 2);
    int length = aux.length();
    char *fpath = new char[length + 1];
    strcpy(fpath, aux.c_str());

    std::string hdfsHost = executor_data->getProperties().hdfsPath();
    int length2 = hdfsHost.length();
    char *chdfsHost = new char[length2 + 1];
    strcpy(chdfsHost, hdfsHost.c_str());
    if (NewHdfsClient(chdfsHost) < 0) {
        throw ignis::executor::core::exception::IException("IO: hdfs client not connected");
        return;
    }
    IGNIS_LOG(info) << "IO: hdfs client connected";
    delete[] chdfsHost;

    auto size = Size(fpath);
    if (size < 0) {
        throw ignis::executor::core::exception::IException("IO: problem opening hdfs file");
        return;
    }
    IGNIS_LOG(info) << "IO: file has " << size << " Bytes";
    auto result = executor_data->getPartitionTools().newPartitionGroup<std::string>();
    decltype(result) thread_groups[io_cores];
    size_t total_bytes = 0;
    size_t elements = 0;

    std::string host = executor_data->getProperties().host();
    
    IGNIS_OMP_EXCEPTION_INIT()
#pragma omp parallel reduction(+ : total_bytes, elements) firstprivate(minPartitions) private(start, end, diff) num_threads(io_cores)
    {
        IGNIS_OMP_TRY()
        auto file = Open(fpath, 'r');
        auto id = executor_data->getContext().threadId();
        auto globalThreadId = executor_data->getContext().executorId() * io_cores + id;
        auto threads = executor_data->getContext().executors() * io_cores;
        size_t ex_chunk = size / threads;
        size_t ex_chunk_init = globalThreadId * ex_chunk;
        size_t ex_chunk_end = ex_chunk_init + ex_chunk;
        size_t minPartitionSize = executor_data->getProperties().partitionMinimal();
        minPartitions = (int64_t) std::ceil(minPartitions / (float) threads);
        std::string str;
        std::vector<std::string> exs;
        std::string ldelim = "\n";
        int esize = 0;
        
        if (globalThreadId > 0) {
            size_t padding = ex_chunk_init >= (ldelim.size()) ? ex_chunk_init - (ldelim.size()) : 0;
            Seek(file, padding, 0);
            while (ex_chunk_init > padding) { padding += std::string((ReadLine(file))).size(); }
            
            ex_chunk_init = padding;
            if (globalThreadId == threads - 1) { ex_chunk_end = size; }
        }
        
        if (ex_chunk / minPartitionSize < minPartitions) { minPartitionSize = ex_chunk / minPartitions; }

        thread_groups[id] = executor_data->getPartitionTools().newPartitionGroup<std::string>();
        auto partition = executor_data->getPartitionTools().newPartition<std::string>();
        auto write_iterator = partition->writeIterator();
        thread_groups[id]->add(partition);
        size_t partitionInit = ex_chunk_init;
        size_t filepos = ex_chunk_init;
        
        if (executor_data->getPartitionTools().isMemory(*partition)) {
            auto part_men = executor_data->getPartitionTools().toMemory(partition);
            while (filepos < ex_chunk_end) {
                if ((filepos - partitionInit) > minPartitionSize) {
                    part_men->fit();
                    part_men = executor_data->getPartitionTools().newMemoryPartition<std::string>();
                    thread_groups[id]->add(part_men);
                    partitionInit = filepos;
                }
                str = ReadLine(file);
                filepos += str.size();
                elements++;
                str.resize(str.size() - ldelim.size());
                part_men->inner().push_back(str);
            }
        } else {
            while (filepos < ex_chunk_end) {
                if ((filepos - partitionInit) > minPartitionSize) {
                    partition->fit();
                    partition = executor_data->getPartitionTools().newPartition<std::string>();
                    write_iterator = partition->writeIterator();
                    thread_groups[id]->add(partition);
                    partitionInit = filepos;
                }
                str = ReadLine(file);
                filepos += str.size();
                elements++;
                str.resize(str.size() - ldelim.size());
                write_iterator->write(str);
            }
        }
        total_bytes += (size_t) filepos - ex_chunk_init;
        
        Close(file, 'r');
        
        IGNIS_OMP_CATCH()
    }
    IGNIS_OMP_EXCEPTION_END()

    delete[] fpath;

    for (auto group : thread_groups) {
        for (auto part : *group) { result->add(part); }
    }

    IGNIS_LOG(info) << "IO: created  " << result->partitions() << " partitions, " << elements << " lines and "
                    << total_bytes << " Bytes read ";

    executor_data->setPartitions(result);

    CloseConnection();
    IGNIS_CATCH()
}

void IIOImpl::textFile(const std::string &path, int64_t minPartitions) {
        if (path.find("hdfs://") == 0) { return hdfsTextFile(path, minPartitions, executor_data, ioCores()); }
        return plainFile(path, minPartitions, "\n");
}

void IIOImpl::partitionTextFile(const std::string &path, int64_t first, int64_t partitions) {
    IGNIS_TRY()
    IGNIS_LOG(info) << "IO: reading partitions text file";
    auto group = executor_data->getPartitionTools().newPartitionGroup<std::string>(partitions);

    IGNIS_OMP_EXCEPTION_INIT()
#pragma omp parallel num_threads(ioCores())
    {
        IGNIS_OMP_TRY()
#pragma omp for schedule(dynamic)
        for (int64_t p = 0; p < partitions; p++) {
            auto file_name = partitionFileName(path, first + p);
            std::ifstream file = openFileRead(file_name);
            auto partition = (*group)[p];
            auto write_iterator = partition->writeIterator();
            std::string buffer;
            while (std::getline(file, buffer, '\n')) { write_iterator->write(buffer); }
            partition->fit();
        }
        IGNIS_OMP_CATCH()
    }
    IGNIS_OMP_EXCEPTION_END()
    executor_data->setPartitions(group);
    IGNIS_CATCH()
}

void IIOImpl::partitionObjectFileVoid(const std::string &path, int64_t first, int64_t partitions) {
    IGNIS_TRY()
    IGNIS_LOG(info) << "IO: reading partitions object file";
    auto group = executor_data->getPartitionTools().newPartitionGroup<IVoidPartition::VOID_TYPE>();

    for (int64_t p = 0; p < partitions; p++) {
        auto file_name = partitionFileName(path, first + p);
        openFileRead(file_name);//Only to check
        auto header = std::make_shared<transport::IFileTransport>(file_name + ".header");
        auto transport = std::make_shared<transport::IFileTransport>(file_name);

        auto partition =
                executor_data->getPartitionTools().newVoidPartition(100 + ghc::filesystem::file_size(file_name));
        partition->read(reinterpret_cast<std::shared_ptr<transport::ITransport> &>(header));
        partition->read(reinterpret_cast<std::shared_ptr<transport::ITransport> &>(transport));

        partition->fit();
        group->add(partition);
    }

    executor_data->setPartitions(group);
    IGNIS_CATCH()
}

class PartitionJsonHandler : public rapidjson::BaseReaderHandler<rapidjson::UTF8<>, PartitionJsonHandler> {
public:
    bool Null() { return assign(IJsonValue()); }

    bool Bool(bool value) { return assign(IJsonValue(value)); }

    bool Int(int value) { return assign(IJsonValue((int64_t) value)); }

    bool Uint(unsigned value) { return assign(IJsonValue((int64_t) value)); }

    bool Int64(int64_t value) { return assign(IJsonValue(value)); }

    bool Uint64(uint64_t value) { return assign(IJsonValue((int64_t) value)); }

    bool Double(double value) { return assign(IJsonValue(value)); }

    bool RawNumber(const Ch *str, rapidjson::SizeType len, bool copy) { return assign(IJsonValue(std::string(str))); }

    bool String(const Ch *str, rapidjson::SizeType len, bool copy) { return assign(IJsonValue(std::string(str))); }

    bool Key(const Ch *str, rapidjson::SizeType len, bool copy) { return assign(IJsonValue(std::string(str))); }

    bool StartObject() {
        bool check = assign(IJsonValue(std::unordered_map<std::string, ignis::executor::api::IJsonValue>()));
        stack.push_back(top);
        return check;
    }

    bool StartArray() {
        bool check = assign(IJsonValue(std::vector<ignis::executor::api::IJsonValue>()));
        stack.push_back(top);
        return check;
    }

    bool EndObject(rapidjson::SizeType memberCount) {
        stack.resize(stack.size() - 1);
        top = stack.back();
        return true;
    }

    bool EndArray(rapidjson::SizeType memberCount) {
        stack.resize(stack.size() - 1);
        top = stack.back();
        return true;
    }

    IJsonValue root() { return rootArray.getArray()[0]; }

    PartitionJsonHandler() {
        rootArray = IJsonValue(std::vector<ignis::executor::api::IJsonValue>());
        stack.push_back(&rootArray);
    }

private:
    bool assign(IJsonValue &&value) {
        if (key.empty()) {
            const_cast<std::vector<IJsonValue> &>(stack[stack.size() - 1]->getArray()).push_back(value);
            top = &const_cast<std::vector<IJsonValue> &>(stack[stack.size() - 1]->getArray()).back();
        } else {
            top = &(const_cast<std::unordered_map<std::string, IJsonValue> &>(
                            stack[stack.size() - 1]->getMap())[std::move(key)] = value);
        }
        return true;
    }

    std::string key;
    IJsonValue rootArray;
    IJsonValue *top;
    std::vector<IJsonValue *> stack;
};

void IIOImpl::partitionJsonFileVoid(const std::string &path, int64_t first, int64_t partitions) {
    IGNIS_TRY()
    IGNIS_LOG(info) << "IO: reading partitions json file";
    auto group = executor_data->getPartitionTools().newPartitionGroup<api::IJsonValue>(partitions);

    IGNIS_OMP_EXCEPTION_INIT()
#pragma omp parallel num_threads(ioCores())
    {
        IGNIS_OMP_TRY()
#pragma omp for schedule(dynamic)
        for (int64_t p = 0; p < partitions; p++) {
            auto file_name = partitionFileName(path, first + p) + ".json";
            std::ifstream file = openFileRead(file_name);
            rapidjson::IStreamWrapper wrapper(file);
            rapidjson::Reader reader;
            PartitionJsonHandler handler;

            if (!reader.Parse<rapidjson::kParseIterativeFlag>(wrapper, handler)) {
                std::string error = rapidjson::GetParseError_En(reader.GetParseErrorCode());

                throw exception::IInvalidArgument(path + " is not valid. " + error + " at offset " +
                                                  std::to_string(reader.GetErrorOffset()));
            }
            if (!handler.root().isArray()) { throw exception::IInvalidArgument(path + " is not a json array"); }

            auto write_iterator = (*group)[p]->writeIterator();
            for (const api::IJsonValue &value : handler.root().getArray()) {
                api::IJsonValue &noconst = const_cast<api::IJsonValue &>(value);
                write_iterator->write(std::move(noconst));
            }
            (*group)[p]->fit();
        }
        IGNIS_OMP_CATCH()
    }
    IGNIS_OMP_EXCEPTION_END()
    executor_data->setPartitions(group);
    IGNIS_CATCH()
}

int IIOImpl::ioCores() {
    double cores = executor_data->getProperties().ioCores();
    if (cores > 1) {
        return std::min(executor_data->getCores(), (int) std::ceil(executor_data->getProperties().ioCores()));
    }
    return std::max(1, (int) std::ceil(executor_data->getProperties().ioCores() * executor_data->getCores()));
}