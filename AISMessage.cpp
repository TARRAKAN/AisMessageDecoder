#include "AISMessage.h"
#include <algorithm>
#include <cmath>

AISMessage::AISMessage(std::string message) {
	message.append(",");
	size_t start = 0;
	size_t end = message.find(",");
	while (end != std::string::npos) {
		aisMessageParts.push_back(message.substr(start, end - start));
		start = end + 1;
		end = message.find(",", start);
	}//Ðàçáèâàåì ñòðîêó ñîîáùåíèÿ íà ÷àñòè, êîòîðûå áûëè ðàçäåëåíû çàïÿòûìè 

	std::transform(aisMessageParts[0].begin(), aisMessageParts[0].end(), aisMessageParts[0].begin(), ::toupper); //Äåëàåì áóêâû òèïà ïàêåòà çàãëàâíûìè
	aisMessageParts.insert(aisMessageParts.begin(),
		aisMessageParts[0].substr(1, 2)); //âûñòàâëÿåì òèï ïåðåäàþùåãî óñòðîéñòâà
	aisMessageParts[SENTTNCE_FORMATTER] = aisMessageParts[SENTTNCE_FORMATTER].substr(3, 3); //Óáèðàåì ïåðâûå ñèìâîëû è îñòàâëÿåì òîëüêî ôîðìàò ñîîáùåíèÿ
	std::transform(aisMessageParts[CHANNEL_CODE].begin(), aisMessageParts[CHANNEL_CODE].end(), //
		aisMessageParts[CHANNEL_CODE].begin(), ::toupper);                                     //Äåëàåì áóêâû êîäà êàíàëà çàãëàâíûìè
}

void AISMessage::setTalkerType(std::string type) {
	std::transform(type.begin(), type.end(), type.begin(), ::toupper);
	bool itIsAValidType = ((type == "AB") ||
		(type == "AD") ||
		(type == "AI") ||
		(type == "AN") ||
		(type == "AR") ||
		(type == "AS") ||
		(type == "AT") ||
		(type == "AX") ||
		(type == "BS") ||
		(type == "SA"));
	if (itIsAValidType) {
		this->aisMessageParts[TALKER_TYPE] = type;
	}
	else throw "Invalid talker type.";
}
std::string AISMessage::getTalkerType() { return aisMessageParts[TALKER_TYPE]; }

void AISMessage::setSentenceFormatter(std::string type) {
	std::transform(type.begin(), type.end(), type.begin(), ::toupper);
	bool itIsAValidType = ((type == "VDM") ||
		(type == "VDO"));
	if (itIsAValidType) {
		aisMessageParts[SENTTNCE_FORMATTER] = type;
	}
	else throw "Invalid sentence formatter.";
}
std::string AISMessage::getSentenceFormatter() {
	return aisMessageParts[SENTTNCE_FORMATTER];
}

void AISMessage::setNumberOfSentences(int numberOfSentences) {
	aisMessageParts[NUMBER_OF_SENTENCES] = numberOfSentences;
}

int AISMessage::getNumberOfSentences() {
	return std::stoi(aisMessageParts[NUMBER_OF_SENTENCES]);
}

void AISMessage::setSentenceNumber(int sentenceNumber) { 
	aisMessageParts[SENTENCE_NUMBER] = sentenceNumber;
}
int AISMessage::getSentenceNumber() {
	return std::stoi(aisMessageParts[SENTENCE_NUMBER]);
}

void AISMessage::setSequentialMessageIdentifier(int sequentialMessageIdentifier) { 
	aisMessageParts[SEQ_MESSAGE_IDENTIFIER] = sequentialMessageIdentifier;
}
int AISMessage::getSequentialMessageIdentifier() {
	return std::stoi(aisMessageParts[SEQ_MESSAGE_IDENTIFIER]);
}

void AISMessage::setChannelCode(std::string code) {
	std::transform(code.begin(), code.end(), code.begin(), ::toupper);
	bool isValid = ((code == "A") ||
		(code == "B"));
	if (isValid) {
		aisMessageParts[CHANNEL_CODE] = code;
	}
	else throw "Invalid channel code.";
}
std::string AISMessage::getChannelCode() { return aisMessageParts[CHANNEL_CODE]; }

void AISMessage::setRawData(std::string data) { aisMessageParts[DATA] = data; }
std::string AISMessage::getRawData() { return aisMessageParts[DATA]; }

void AISMessage::setCheckSum(std::string checkSum) { aisMessageParts[CHECKSUM] = checkSum; }
std::string AISMessage::getCheckSum() { return aisMessageParts[CHECKSUM]; }

std::string AISMessage::getAISMessage() {
	std::string stringToReturn = "!" + aisMessageParts[TALKER_TYPE] +
	 	aisMessageParts[SENTTNCE_FORMATTER];
	for (size_t i = 2; i < aisMessageParts.size(); ++i) {
		stringToReturn += ",";
		stringToReturn += aisMessageParts[i];
	}
	return stringToReturn;
}

char AISMessage::ascii8To6Bit(char ch){
        ch-=48;
        if(ch < 40)
            return ch;
        else 
            return ch - 8;
}

char AISMessage::ascii6To8Bit(char ch){
        if(ch < 40)
        	return ch + 48;
        else
        	return ch + 56;
}

unsigned short AISMessage::getMessageType(){
	unsigned short type = AISMessage::ascii8To6Bit(aisMessageParts[DATA].front());
	if (type > 27)
		throw "Invalid message type."; //заменить на std::Excettion;
	else
		return type;
}

std::string AISMessage::getBinaryMessage(){
    std::string msgDta = aisMessageParts[DATA];
    std::bitset<6> bs;
    std::string retStr = "";
    for(auto i = msgDta.begin(); i < msgDta.end(); i++){
        *i = AISMessage::ascii8To6Bit(*i);
        bs = *i;
        retStr+=bs.to_string();
    }
    return retStr;
}


