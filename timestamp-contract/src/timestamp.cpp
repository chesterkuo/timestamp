#include <eosio/print.hpp>
#include <eosio/eosio.hpp>
#include <eosio/system.hpp>
#include <eosio/transaction.hpp>
#include "timestamp.hpp"

using namespace eosio;

namespace timestamp {

    uuid_type get_global_id(name user) {
         uint128_t self_128 = static_cast<uint128_t>(user.value);
         uint128_t id_128 = static_cast<uint128_t>(user.value);
         uuid_type res = (self_128 << 64) | (id_128);
         return res;
    }

    [[eosio::action]]
    void timestamp::delstamp(name scope, const uint64_t id) {
        //erase event
        require_auth(_self);

	timestamp_index timestamp_idx(get_self(), scope.value);
        auto itr = timestamp_idx.find(id);
        check(itr != timestamp_idx.end(), "TimeStamp id not found for erasing");

        //erase entry
        timestamp_idx.erase(itr);
    }

    [[eosio::action]]
    void timestamp::addstamp(bool write, name scope, const string entity, const string jsons) { 
        require_auth(_self);
	if (write == false) {
            print(" skip write to state db \n");   
	    return;
	}

	uint128_t id = 0;
	uint64_t idx = 0;
        auto size = transaction_size();
        checksum256 h;
        char buf[size];
        uint32_t read = read_transaction( buf, size );
        check( size == read, "read_transaction failed");
        h = sha256(buf, read);
        auto hbytes = h.extract_as_byte_array();
	// need 64/128 bits
        for(int i=0; i<16; i++) {
	    if (i < 8){
                idx <<=8;
		idx |= hbytes[i];
            }
	    id <<=8;
	    id |= hbytes[i];
        }

	timestamp_index timestamp_idx(get_self(), scope.value);
        auto iterator = timestamp_idx.find(idx);

        if (iterator == timestamp_idx.end()) {
          printf(" create entry for entry\n");
          timestamp_idx.emplace(_self, [&]( auto& row ) {
              row.idx = idx;
	      row.id = id;
              row.jsons = jsons;
	      row.entity = entity;
              row.created_at = time_point_sec(current_time_point().sec_since_epoch());
              row.updated_at = time_point_sec(current_time_point().sec_since_epoch());
          });
        } else {
            printf(" update entry for event\n");
            timestamp_idx.modify(iterator, _self, [&] (auto & row) {
               row.jsons = jsons;
	       row.entity = entity;
               row.updated_at = time_point_sec(current_time_point().sec_since_epoch());
	    });
	}
    }
    [[eosio::action]]
    void timestamp::lock(uint64_t value) {
        require_auth(_self);

	/*
	frozen frozen_t = get_lock_flag();
	frozen_t.frozen = value;
	_frozen.set(frozen_t, get_self());
	*/
    }
    /*
    uint64_t timestamp::get_lock_flag() {
	frozen frozen_t{.frozen=0};
	return _frozen.get_or_create(get_self(),frozen_t);
    }
    */

} // namespace 


extern "C" {
    void apply(uint64_t receiver, uint64_t code, uint64_t action) {
        auto self = receiver;
    
        if (code == self) {
            switch (action) { 
                EOSIO_DISPATCH_HELPER(timestamp::timestamp, (delstamp)(addstamp)(lock)) 
            }    
        }
        eosio_exit(0);
    }
}

