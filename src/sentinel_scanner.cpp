#include "core/sentinel_scanner.h"


    SentinelScanner::SentinelScanner(std::string sentinel){
        sentinel_ = sentinel;
    }

    //struct Out { std::string safe_text; bool sentinel_found; };

    // Feed the next chunk. Returns text guaranteed NOT to be part of
    // the sentinel (safe to print immediately) and whether the
    // sentinel has now been fully seen.
    SentinelScanner::Out SentinelScanner::feed(std::string_view chunk){
        std::string combination = pending_ + std::string(chunk);
        size_t position = combination.find(sentinel_);
        Out safeString = {"", false}; //initialization
        if(position != std::string::npos){ // if the sentinel is found
            std::string lastString = combination.substr(0, position);
            safeString = {lastString, true};  // everything before it is safe, and the sentinel is found
            pending_.clear(); // get rid of anything else (the sentinel itself)
        }
        else{ //the full sentinel is not found
            if(combination.size() >= sentinel_.size()){  //if the combination of pending and the new chunck is longer than the sentinel
                std::string not_sentinel = combination.substr(0, combination.size() - sentinel_.size() + 1); // take what is before the sentinel size - 1 last chars
                safeString = {not_sentinel, false};
                std::string newPending = combination.substr(combination.size() - sentinel_.size() + 1, combination.size());//take what is left over to be pending for the next time
                pending_ = newPending;
            }
            else{
                pending_ = combination;
            }
        }
        return safeString;  //output the struct with safe words and if the sentinel has been seen yet or not
    }

    // Call once, after the stream ends, to release any text still
    // being held back.
    SentinelScanner::Out SentinelScanner::flush(){
        std::string leftover = pending_;
        pending_.clear();
        return {leftover, false};
    }