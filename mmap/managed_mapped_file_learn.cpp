#include <iostream>

#include <boost/interprocess/allocators/node_allocator.hpp>
#include <boost/interprocess/managed_mapped_file.hpp>
#include <boost/interprocess/containers/map.hpp>
#include <boost/interprocess/containers/string.hpp>
#include <boost/interprocess/containers/vector.hpp>
#include <boost/interprocess/offset_ptr.hpp>

#include <boost/container/string.hpp>
#include <boost/lexical_cast.hpp>

namespace bipc = ::boost::interprocess;
typedef bipc::managed_mapped_file managed_mapped_file_t;
typedef bipc::managed_mapped_file::segment_manager mapped_segment_manager_t;

typedef bipc::node_allocator<float, mapped_segment_manager_t> vec_allocator_t;
typedef bipc::vector<float, vec_allocator_t> vector_t;

struct Msg
{
    Msg(const vec_allocator_t &vec_alloc) : score(vec_alloc){}

    uint32_t id;
    uint32_t age;
    vector_t score;
};

typedef std::pair<const uint32_t, Msg> pair_t;
typedef bipc::node_allocator<pair_t, mapped_segment_manager_t> allocator_t;
typedef std::less<uint32_t> less_t;

typedef bipc::map<uint32_t, Msg, less_t, allocator_t> msg_map_t;
typedef msg_map_t::iterator map_iter_t;

int main()
{
    managed_mapped_file_t obj_mapped_file(bipc::open_or_create, "./msg_map_vector_construct.mmap", 1024*1024);
    msg_map_t *p_msg_map = obj_mapped_file.find_or_construct<msg_map_t>("msg_map")(less_t(), obj_mapped_file.get_segment_manager());
    if(NULL == p_msg_map)
    {
        std::cerr<<"construct msg_map failed"<<std::endl;
        return -1;
    }

    vec_allocator_t obj_alloc(obj_mapped_file.get_segment_manager());

    for(int i = 0; i < 10; ++i)
    {
        map_iter_t itr = p_msg_map->find(i);
        if(itr == p_msg_map->end())
        {
            std::cout<<"not find:"<<i<<" insert:"<<i<<std::endl;

            Msg msg(obj_alloc);
            msg.id = i;
            msg.age = 100 +i;
            msg.score.push_back(i);
            msg.score.push_back(i + 1);

            p_msg_map->insert(std::pair<uint32_t, Msg>(i, msg));
        }
        else
        {
            std::cout<<"find:"<<i<<" data:"<<itr->second.age;
            std::cout<<" score:";
            for(int j = 0; j < itr->second.score.size(); ++j)
                std::cout<<itr->second.score[j]<<" ";
            std::cout<<std::endl;
        }
    }

    return 0;
}
