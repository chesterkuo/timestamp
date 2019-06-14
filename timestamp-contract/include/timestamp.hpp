#pragma once

#include <eosio/print.hpp>
#include <eosio/eosio.hpp>
#include <eosio/asset.hpp>
#include <eosio/singleton.hpp>
#include <eosio/crypto.hpp>
#include <string>
#include <vector>
#include <ctime>
#include <cmath>

using namespace eosio;
using std::string;
using std::vector;

typedef uint128_t uuid_type;
typedef string ipfshash_t;

namespace timestamp {

    class [[eosio::contract("timestamp")]] timestamp : public eosio::contract {

      public:
        using contract::contract;

        static fixed_bytes<32> ipfs_to_fixed_bytes32(const ipfshash_t& input) {
          return sha256(input.c_str(), input.size());
        }

        [[eosio::action]] void delstamp(name scope, const uint64_t id); 
        [[eosio::action]] void addstamp(bool write, name scope, const string entity, const string jsons);
        [[eosio::action]] void lock(uint64_t value);


      private:

	//uint64_t get_lock_flag();

        struct [[eosio::table, eosio::contract("timestamp")]] timestamp_info {
          uint64_t		idx;
	  uint128_t             id;
          time_point_sec	created_at;
          time_point_sec	updated_at;
	  string		entity;
          string		jsons;
          //ipfshash_t		file_hash;
          uint64_t		primary_key() const { return idx; }
	  uint128_t             get_id() const { return id; }
	  uint64_t              get_updated() const { return updated_at.utc_seconds; }
        };


        struct[[eosio::table, eosio::contract("timestamp")]] frozen {
          uint64_t	frozen;
        };


        typedef eosio::multi_index<
		"timestamp"_n, timestamp_info,
                indexed_by< "byid"_n, const_mem_fun<timestamp_info, uint128_t, &timestamp_info::get_id>>,
		indexed_by< "byexpires"_n, const_mem_fun<timestamp_info, uint64_t, &timestamp_info::get_updated>>> timestamp_index;
        typedef eosio::singleton<"freeze"_n, frozen> frozen_table;

	//frozen_table _frozen;
    };

} //namespace ndoered 
