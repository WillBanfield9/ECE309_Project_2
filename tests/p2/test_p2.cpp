// tests/p2/test_p2.cpp
//
// YOUR test suite goes here. At least 12 assert-based test cases — see
// spec §5 for the required categories and the sample test for the
// expected level of rigor.
//
// This file is a stub so the project builds out of the box; replace the
// body of main() with your own tests.

#include "core/conversation.h"
#include "core/message.h"
#include "core/sentinel_scanner.h"
#include "harness/harness.h"
#include "model/replay_client.h"
#include "model/scripted_client.h"

#include <cassert>
#define TEST(function) void function()

// 1. **Empty Conversation Bounds:** Handle empty conversations without out-of-bounds access.
// 2. **System Message Ordering:** Ensure system messages remain pinned at the front.
// 3. **Rule of Five (Copy):** Assert copy constructors allocate entirely different pointer addresses.
// 4. **Rule of Five (Move):** Assert move constructors steal the data pointer and zero the source.
// 5. **Growth behavior:** Assert capacity grows per your documented growth factor and `size()`/`at()` stay correct across reallocation.
// 6. **Scanner (Clean Text):** Verify the scanner processes strings with no sentinel correctly.
// 7. **Scanner (Split Sentinel):** Prove the scanner catches the sentinel split across *every possible boundary* (loop over all split points programmatically).
// 8. **Scanner (False Alarms):** Ensure the scanner doesn't trigger on partial matches (e.g., `<|end_world|>`).
// 9. **Scanner (Bounded Memory):** Assert `pending_` never exceeds `sentinel.size() - 1` while feeding a large adversarial stream.
// 10. **Harness (Turn Limit):** Confirm the provided loop stops with `TurnLimit` when your `Conversation` is used underneath it.
// 11. **Harness (Sentinel Halt):** Confirm the provided loop halts exactly when your `SentinelScanner` reports the sentinel found.
// 12. **Transcript Round-Trip:** Save a mock conversation, load it via the provided `ReplayModelClient`, assert identical playback.

//////////////////////////////////////////////////////
//Test 1
TEST(HandleEmptyConversations) {
    Conversation empty_conv;
    assert(empty_conv.Conversation::size() == 0);
}

//Test 2
TEST(SystemAtFront) {
    Conversation convo;
    const Message systemMessage;
    const Message userMessage(Role::User, "user");
    const Message assistantMessage(Role::Assistant, "asst");
    convo.Conversation::append(systemMessage);
    convo.Conversation::append(userMessage);
    convo.Conversation::append(assistantMessage);
    assert(convo.Conversation::at(0).role() == Role::System);
}

//Test 3
TEST(ROFCopy){
    Conversation firstConvo;
    Message m;
    firstConvo.append(m);
    Conversation copyConvo = firstConvo;
    assert(firstConvo.begin() != copyConvo.begin()); // Asserts that starting pointer to the first message in the copy is not the same as in the first one
}

//Test 4
TEST(ROFMove) {
    Conversation original;
    Message m;
    original.append(m);
    const Message* temporaryPointer = original.begin();
    Conversation newVersion = std::move(original);
    assert(newVersion.begin() == temporaryPointer);
    assert(original.size() == 0);
}

//Test 5 
TEST(GrowthBehavior) {
    Conversation convo;
    Message m(Role::System, "");
    Message n(Role::User, "");
    Message o(Role::Assistant,"");
    convo.append(m);
    convo.append(n);
    assert(convo.size() == 2);
    convo.append(o);
    assert(convo.size() == 3);  //checks that size is preserved across copying
    assert(convo.capacity() == 4);  //checks that the growth factor is right
    assert(convo.at(0).role() == Role::System);  //these three lines check that .at is preserved across copying
    assert(convo.at(1).role() == Role::User);
    assert(convo.at(2).role() == Role::Assistant);
}

//Test 6
TEST(CleanScan) {
     const std::string sentinel = "<|end_conversation|>";
     SentinelScanner scanner(sentinel);
     const std::string text ="asdfghjkjytrewasdfghtrewsdfghytrewsdfhjytrewsdfghjuytresdfghytrewsdfghjuytresdfhjytr";  //text without sentinel
     std::size_t split = 7;
     for(std::size_t i =0; i<text.size(); i += split){
        assert(scanner.feed(text.substr(i, i+split)).sentinel_found == false);  //asserts that the sentinel is never found
     }

}

//Test 7
TEST(ScannerCatchesSentinelAtEveryBoundary) {
    const std::string sentinel = "<|end_conversation|>";
    const std::string text = "Goodbye." + sentinel;
    for (std::size_t split = 0; split <= text.size(); ++split) {
        SentinelScanner scanner(sentinel);
        auto out1 = scanner.feed(text.substr(0, split));
        auto out2 = scanner.feed(text.substr(split));
        assert((out1.sentinel_found || out2.sentinel_found) &&
               "sentinel must be caught regardless of split point");
        assert(out1.safe_text + out2.safe_text == "Goodbye.");
    }
}

//Test 8 
TEST(FalseAlarms){
    const std::string sentinel = "<|end_conversation|>";
    SentinelScanner scanner(sentinel);
    const std::string false1 = "<|end_conversation|"; //almost a match
    const std::string false2 = "<|end_world|>"; //not a match either
    assert(scanner.feed(false1).sentinel_found == false); //makes sure not to match with close call
    assert(scanner.feed(false2).sentinel_found == false);  // doesnt match with wrong message
}

//Test 9
TEST(BoundedPending){
    const std::string sentinel = "<|end_conversation|>";
    SentinelScanner scanner(sentinel);
    const std::string text ="asdfghjkjytrewasdfghtrewsdfghytrewsdfhjytrewsdfghjuytresdfghytrewsdfghjuytresdfhjytr oh and here are some actual words. This makes it a very long string to scan, but we will break it up with chunks. The chunks will still be large though so as not to bias the test. <|end_conversation|>";
    std::size_t split = 17;
    for(std::size_t i =0; i<text.size(); i += split){
        assert(scanner.pendingSize() < sentinel.size()-1);  //asserts that the sentinel is never found
        split++; //this way sometimes it is shorter than pending, sometimes it is longer
    }

}

//TeST 10
TEST(TurnLimit){
    
}



int main() {

    HandleEmptyConversations();                 //TEST 1   
    SystemAtFront();                            //TEST 2
    ROFCopy();                                  //TEST 3
    ROFMove();                                  //TEST 4
    GrowthBehavior();                           //TEST 5
    CleanScan();                                //TEST 6
    ScannerCatchesSentinelAtEveryBoundary();    //TEST 7
    FalseAlarms();                              //TEST 8
    BoundedPending();                           //TEST 9
    return 0;
}
