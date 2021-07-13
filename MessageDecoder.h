#ifndef MESSAGE_DECODER_H
#define MESSAGE_DECODER_H

#include <map>
#include "AISMessage.h"

class MessageDecoder{
    std::vector<AISMessage> vMessages;
    char bitsetToChar(std::bitset<6> bitset);
    std::string bitsToString(unsigned long firstBit, unsigned long size);
    std::string bitsToEPFD(unsigned long firstBit);

    std::string getBitsFromMessages();

    std::string bitsToMonths(unsigned long firstBit);

    std::string bitsToDay(unsigned long firstBit);

    std::string bitsToHour(unsigned long firstBit);

    std::string bitsToMinute(unsigned long firstBit);

    std::string bitsToBool(unsigned long firstBit);
public:
    MessageDecoder(AISMessage message);
    MessageDecoder(std::vector<AISMessage> messages);

    void addMessage(AISMessage message);
    
    std::map<std::string, std::string> getMessageInfo();
};

#endif //MASSAGE_DECODER_H;