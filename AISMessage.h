#ifndef AIS_MASSAGE_H
#define AIS_MASSAGE_H

#include <string>
#include <vector>
#include <bitset>
#include <map>

class AISMessage {
	std::vector<std::string> aisMessageParts;
	#define TALKER_TYPE 0
	#define SENTTNCE_FORMATTER 1
	#define NUMBER_OF_SENTENCES 2
	#define SENTENCE_NUMBER 3
	#define SEQ_MESSAGE_IDENTIFIER 4
	#define CHANNEL_CODE 5
	#define DATA 6
	#define CHECKSUM 7

	//		AB, // NMEA 4.0 Base AIS station
	//		AD, // NMEA 4.0 Dependent AIS Base Station
	//		AI,	// Mobile AIS station
	//		AN, // NMEA 4.0 Aid to Navigation AIS station
	//		AR, // NMEA 4.0 AIS Receiving Station
	//		AS, // NMEA 4.0 Limited Base Station
	//		AT, // NMEA 4.0 AIS Transmitting Station
	//		AX, // NMEA 4.0 Repeater AIS station
	//		BS, // Base AIS station (deprecated in NMEA 4.0)
	//		SA, // NMEA 4.0 Physical Shore AIS Station


public:
	AISMessage(std::string message);

	//Äàëåå èäóò ñåòåðû è ãåòåðû ïðèâàòíûõ ïîëåé êëàññà
	void setTalkerType(std::string type);
	std::string getTalkerType();

	void setSentenceFormatter(std::string type); //выбрасывать исключения а не вool
	std::string getSentenceFormatter();

	void setNumberOfSentences(int numberOfSentences);
	int getNumberOfSentences();

	void setSentenceNumber(int sentenceNumber);
	int getSentenceNumber();

	void setSequentialMessageIdentifier(int sequentialMessageIdentifier);
	int getSequentialMessageIdentifier();

	void setChannelCode(std::string code);
	std::string getChannelCode();

	void setRawData(std::string data);
	std::string getRawData();

	void setCheckSum(std::string checkSum);
	std::string getCheckSum();

	std::string getAISMessage();
    // Методы для работы с 
    static char ascii8To6Bit(char ch); // 8bit ascii to 6bit ascii
    static char ascii6To8Bit(char ch); // 6bit ascii to 8bit ascii

    unsigned short getMessageType();

    std::string getBinaryMessage();

    //создать методы для перевода bs  в int & uint

    std::map<std::string, std::string> getMessageInfo();
};

#endif // AIS_MASSAGE_H