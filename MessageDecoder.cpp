#include <cmath>
#include "MessageDecoder.h"

MessageDecoder::MessageDecoder(AISMessage message){
    vMessages.push_back(message);
}

MessageDecoder::MessageDecoder(std::vector<AISMessage> messages){
    vMessages = messages;
}

void MessageDecoder::addMessage(AISMessage message){
    vMessages.push_back(message);
}

char MessageDecoder::bitsetToChar(std::bitset<6> bitset){
    char ASCIIchars[] = {'@','A','B','C','D','E','F','G','H','I','J','K','L','M','N','O',
                       'P','Q','R','S','T','U','V','W','X','Y','Z','[','\\',']','^','_',
                       ' ','!','\"','#','$','%','&','\'','(',')','*','+',',','-','.','/',
                       '0','1','2','3','4','5','6','7','8','9',':',';','<','=','>','?'};
    return ASCIIchars[bitset.to_ulong()];
}

std::string MessageDecoder::getBitsFromMessages(){
    std::string bits = "";
    for(AISMessage msg: vMessages){
        bits += msg.getBinaryMessage();
    }
    return bits;
}

std::string MessageDecoder::bitsToString(unsigned long firstBit, unsigned long size)
{
        std::string str = "";
        for(int i = firstBit; i < firstBit + size; i+=6){
            std::bitset<6> bsChar(this->getBitsFromMessages(), i, 6);
            if(bitsetToChar(bsChar) != '@') //'@' rem
                str += bitsetToChar(bsChar);
        }
        return str;
}

std::string MessageDecoder::bitsToMonths(unsigned long firstBit){
    std::bitset<4> bsMonths(this->getBitsFromMessages(), firstBit, 4);
    return (bsMonths.to_ulong() == 0) ? "not available" : std::to_string(bsMonths.to_ulong());
}

std::string MessageDecoder::bitsToDay(unsigned long firstBit){
    std::bitset<5> bsDay(this->getBitsFromMessages(), firstBit, 5);
    return (bsDay.to_ulong() == 0) ? "not available" : std::to_string(bsDay.to_ulong());
}

std::string MessageDecoder::bitsToHour(unsigned long firstBit){
    std::bitset<5> bsHour(this->getBitsFromMessages(), firstBit, 5);
    return (bsHour.to_ulong() == 24) ? "not available" : std::to_string(bsHour.to_ulong());
}

std::string MessageDecoder::bitsToMinute(unsigned long firstBit){
    std::bitset<6> bsMinute(this->getBitsFromMessages(), firstBit, 6);
    return (bsMinute.to_ulong() == 60) ? "not available" : std::to_string(bsMinute.to_ulong());
}

std::string MessageDecoder::bitsToEPFD(unsigned long firstBit){
    std::bitset<4> bsEPFD(this->getBitsFromMessages(), firstBit, 4);
        switch(bsEPFD.to_ulong()){
            case 0:return "Undefined";
            case 1:return "GPS";
            case 2:return "GLONASS";
            case 3:return "Combined GPS/GLONASS";
            case 4:return "Loran-C";
            case 5:return "Chayka";
            case 6:return "Integrated navigation system";
            case 7:return "Surveyed";
            case 8:return "Galileo";
            default:return "not used";
        }
}

std::string MessageDecoder::bitsToBool(unsigned long firstBit){
    std::bitset<1> bsBool(this->getBitsFromMessages(), firstBit, 1); //accuracy
    return std::to_string(bsBool.to_ulong()); 
}


std::map<std::string, std::string> MessageDecoder::getMessageInfo(){
    std::map<std::string, std::string> mapToReturn;
    mapToReturn["type"] = std::to_string(vMessages[0].getMessageType());//type

    std::bitset<2> bsRepIndic(vMessages[0].getBinaryMessage(), 6, 2);//repeat
    mapToReturn["repeat"] = std::to_string(bsRepIndic.to_ulong());

    std::bitset<30> bsMMSI(vMessages[0].getBinaryMessage(), 8, 30);//mmsi
    mapToReturn["mmsi"] = std::to_string(bsMMSI.to_ulong());
    if(vMessages[0].getMessageType() == 1 ||
       vMessages[0].getMessageType() == 2 || 
       vMessages[0].getMessageType() == 3 )
    {   
        std::bitset<4> bsStatus(vMessages[0].getBinaryMessage(), 38, 4);//status
        mapToReturn["status"] = std::to_string(bsStatus.to_ulong());
        switch(std::stoi(mapToReturn["status"])){
            case 0: mapToReturn["status"] = "under way using engine"; break;
            case 1: mapToReturn["status"] = "at anchor"; break;
            case 2: mapToReturn["status"] = "not under command"; break;
            case 3: mapToReturn["status"] = "restricted manoeuverability"; break;
            case 4: mapToReturn["status"] = "constrained by her draught"; break;
            case 5: mapToReturn["status"] = "moored"; break;
            case 6: mapToReturn["status"] = "aground"; break;
            case 7: mapToReturn["status"] = "engaged in Fishing"; break;
            case 8: mapToReturn["status"] = "under way sailing"; break;
            case 9: mapToReturn["status"] = 
                "reserved for future amendment of Navigational Status for HSC"; break;
            case 10: mapToReturn["status"] =
                "reserved for future amendment of Navigational Status for WIG"; break;
            case 11:;
            case 12:;
            case 13: mapToReturn["status"] = "reserved for future use"; break;
            case 14: mapToReturn["status"] = "AIS-SART is active"; break;
            case 15: mapToReturn["status"] = "not defined (default)"; break;
        }

        std::bitset<8> bsTurn(vMessages[0].getBinaryMessage(), 42, 8); //turn
        if(bsTurn[7] == 1){
            bsTurn = bsTurn.to_ulong() - 1;
            bsTurn.flip();
            mapToReturn["turn"] = std::to_string((-1) * pow((long)bsTurn.to_ulong() / 4.733, 2)); 
        }
        else{
            mapToReturn["turn"] = std::to_string(bsTurn.to_ulong());
        }
        if(mapToReturn["turn"] == "720.003211"){ // подумать
            mapToReturn["turn"] = "turning right at more than 5deg/30s (No TI available)";
        }
        if (mapToReturn["turn"] == "-720.003211")// подумать
        {
            mapToReturn["turn"] = "turning left at more than 5deg/30s (No TI available)";
        }
        if (mapToReturn["turn"] == "-731.386484" ||
            mapToReturn["turn"] == "731.386484")
        {
            mapToReturn["turn"] = "not available";
        }
           

        std::bitset<10> bsSpeed(vMessages[0].getBinaryMessage(), 50, 10);//speed
        mapToReturn["speed"] = std::to_string(bsSpeed.to_ulong()*0.1);
        if(mapToReturn["speed"] == "102.2"){
            mapToReturn["speed"] = "102.2 knots or higher";
        }
        if(mapToReturn["speed"] == "102.3"){
            mapToReturn["speed"] = "not available";
        }

        std::bitset<1> bsAccuracy(vMessages[0].getBinaryMessage(), 60, 1); //accuracy
        mapToReturn["accuracy"] = std::to_string(bsAccuracy.to_ulong());

        std::bitset<28> bsLongitude(vMessages[0].getBinaryMessage(), 61, 28);//lon
        if(bsLongitude[27] == 1){
            bsLongitude = bsLongitude.to_ulong() - 1;
            bsLongitude.flip();
            mapToReturn["longitude"] = std::to_string((-1) * (long double)bsLongitude.to_ulong()
             / 600000); 
        }
        else{
            mapToReturn["longitude"] = std::to_string(((long double)bsLongitude.to_ulong() 
                / 600000));
        }
        if(mapToReturn["longitude"] == "181" ||
           mapToReturn["longitude"] == "-181"){
            mapToReturn["longitude"] = "not available";
        }

        std::bitset<27> bsLatitude(vMessages[0].getBinaryMessage(), 89, 27);//lat
        if(bsLongitude[27] == 1){
            bsLatitude = bsLatitude.to_ulong() - 1;
            bsLatitude.flip();
            mapToReturn["latitude"] = std::to_string((-1) * (long double)bsLatitude.to_ulong()
             / 600000); 
        }
        else{
            mapToReturn["latitude"] = std::to_string(((long double)bsLatitude.to_ulong() / 600000));
        }
        if(mapToReturn["latitude"] == "91" ||
           mapToReturn["latitude"] == "-91"){
            mapToReturn["latitude"] = "not available";
        }
        
        std::bitset<12> bsCourse(vMessages[0].getBinaryMessage(), 116, 12);//course
        mapToReturn["course"] = std::to_string((double)bsCourse.to_ulong() * 0.1);
        if(mapToReturn["course"] == "3600"){
            mapToReturn["course"] = "not available";
        }

        std::bitset<9> bsHeading(vMessages[0].getBinaryMessage(), 128, 9); //heading
        mapToReturn["heading"] = std::to_string(bsHeading.to_ulong());
        if(mapToReturn["heading"] == "511"){
            mapToReturn["heading"] = "not available";
        }

        std::bitset<6> bsSecond(vMessages[0].getBinaryMessage(), 137, 6);//second
        mapToReturn["second"] = std::to_string(bsSecond.to_ulong());
        switch(std::stoi(mapToReturn["second"])){
            case 60:mapToReturn["second"] = "time stamp is not available";break;
            case 61:mapToReturn["second"] = "positioning system is in manual input mode";break;
            case 62:mapToReturn["second"] = 
                "Electronic Position Fixing System operates in estimated (dead reckoning) mode";break;
            case 63:mapToReturn["second"] = "positioning system is inoperative";break;
        }

        std::bitset<2> bsManeuver(vMessages[0].getBinaryMessage(), 143, 2);//Maneuver
        mapToReturn["maneuver"] = std::to_string(bsManeuver.to_ulong());
        switch(std::stoi(mapToReturn["maneuver"])){
            case 0:mapToReturn["maneuver"] = "not available";break;
            case 1:mapToReturn["maneuver"] = "no special maneuver";break;
            case 2:mapToReturn["maneuver"] = "special maneuver";break;
        }

        //spareXXXXXXXX

        mapToReturn["raim"] = bitsToBool(148);//RAIM

        std::bitset<19> bsRadio(vMessages[0].getBinaryMessage(), 149, 19); //radio
        mapToReturn["radio"] = std::to_string(bsRadio.to_ulong());
    }
    else if(vMessages[0].getMessageType() == 4){
        std::bitset<14> bsYear(vMessages[0].getBinaryMessage(), 38, 14);//year
        mapToReturn["year"] = std::to_string(bsYear.to_ulong());
        if(mapToReturn["year"] == "0"){
            mapToReturn["year"] = "not available";
        }

        mapToReturn["month"] = bitsToMonths(52); //CF

        mapToReturn["day"] = bitsToDay(56); //CF

        mapToReturn["hour"] = bitsToHour(61); //CF

        mapToReturn["minute"] = bitsToMinute(66); //CF

        std::bitset<6> bsSecond(vMessages[0].getBinaryMessage(), 72, 6); //second
        mapToReturn["second"] = std::to_string(bsSecond.to_ulong());
        if(mapToReturn["second"] == "60"){
            mapToReturn["second"] = "not available";
        }

        mapToReturn["accuracy"] = bitsToBool(78);//accuracy

        std::bitset<28> bsLongitude(vMessages[0].getBinaryMessage(), 79, 28);//lon
        if(bsLongitude[27] == 1){
            bsLongitude = bsLongitude.to_ulong() - 1;
            bsLongitude.flip();
            mapToReturn["longitude"] = std::to_string((-1) * (long double)bsLongitude.to_ulong()
             / 600000); 
        }
        else{
            mapToReturn["longitude"] = std::to_string(((long double)bsLongitude.to_ulong() 
                / 600000));
        }
        if(mapToReturn["longitude"] == "181" ||
           mapToReturn["longitude"] == "-181"){
            mapToReturn["longitude"] = "not available";
        }

        std::bitset<27> bsLatitude(vMessages[0].getBinaryMessage(), 107, 27);//lat
        if(bsLongitude[27] == 1){
            bsLatitude = bsLatitude.to_ulong() - 1;
            bsLatitude.flip();
            mapToReturn["latitude"] = std::to_string((-1) * (long double)bsLatitude.to_ulong()
             / 600000); 
        }
        else{
            mapToReturn["latitude"] = std::to_string(((long double)bsLatitude.to_ulong() / 600000));
        }
        if(mapToReturn["latitude"] == "91" ||
           mapToReturn["latitude"] == "-91"){
            mapToReturn["latitude"] = "not available";
        }

        mapToReturn["epfd"] = bitsToEPFD(134);//EPFD

        //spareXXXXXXXXXXXXXXXX

        mapToReturn["raim"] = bitsToBool(149);//RAIM

        std::bitset<19> bsRadio(vMessages[0].getBinaryMessage(), 149, 19); //radio
        mapToReturn["radio"] = std::to_string(bsRadio.to_ulong());

    }
    else if(vMessages[0].getMessageType() == 5){
        std::bitset<2> bsVersion(vMessages[0].getBinaryMessage(), 38, 2);//ais_version
        mapToReturn["ais_version"] = std::to_string(bsVersion.to_ulong());

        std::bitset<30> bsImo(vMessages[0].getBinaryMessage(), 40, 30);//ais_version
        mapToReturn["imo"] = std::to_string(bsImo.to_ulong());

        
        mapToReturn["callsign"] = bitsToString(70, 42);

        mapToReturn["shipname"] = bitsToString(112, 120);

        std::bitset<8> bsShiptype(vMessages[0].getBinaryMessage(), 232, 8);
        switch(bsShiptype.to_ulong()){
            case 0:mapToReturn["shiptype"] = "Not available";
            case 1:;
            case 2:;
            case 3:;
            case 4:;
            case 5:;
            case 6:;
            case 7:;
            case 8:;
            case 9:;
            case 10:;
            case 11:;
            case 12:;
            case 13:;
            case 14:;
            case 15:;
            case 16:;
            case 17:;
            case 18:;
            case 19:mapToReturn["shiptype"] = "Reserved for future use";break;
            case 20:mapToReturn["shiptype"] = "Wing in ground (WIG), all ships of this type";break;
            case 21:mapToReturn["shiptype"] = "Wing in ground (WIG), Hazardous category A";break;
            case 22:mapToReturn["shiptype"] = "Wing in ground (WIG), Hazardous category B";break;
            case 23:mapToReturn["shiptype"] = "Wing in ground (WIG), Hazardous category C";break;
            case 24:mapToReturn["shiptype"] = "Wing in ground (WIG), Hazardous category D";break;
            case 25:;
            case 26:;
            case 27:;
            case 28:;
            case 29:mapToReturn["shiptype"] = "Wing in ground (WIG), Reserved for future use";break;
            case 30:mapToReturn["shiptype"] = "Fishing";break;
            case 31:mapToReturn["shiptype"] = "Towing";break;
            case 32:mapToReturn["shiptype"] = "Towing: length exceeds 200m or breadth exceeds 25m";break;
            case 33:mapToReturn["shiptype"] = "Dredging or underwater ops";break;
            case 34:mapToReturn["shiptype"] = "Diving ops";break;
            case 35:mapToReturn["shiptype"] = "Military ops";break;
            case 36:mapToReturn["shiptype"] = "Sailing";break;
            case 37:mapToReturn["shiptype"] = "Pleasure Craft";break;
            case 38:;
            case 39:mapToReturn["shiptype"] = "Reserved";break;
            case 40:mapToReturn["shiptype"] = "High speed craft (HSC), all ships of this type";break;
            case 41:mapToReturn["shiptype"] = "High speed craft (HSC), Hazardous category A";break;
            case 42:mapToReturn["shiptype"] = "High speed craft (HSC), Hazardous category B";break;
            case 43:mapToReturn["shiptype"] = "High speed craft (HSC), Hazardous category C";break;
            case 44:mapToReturn["shiptype"] = "High speed craft (HSC), Hazardous category D";break;
            case 45:;
            case 46:;
            case 47:;
            case 48:mapToReturn["shiptype"] = "High speed craft (HSC), Reserved for future use";break;
            case 49:mapToReturn["shiptype"] = "High speed craft (HSC), No additional information";break;
            case 50:mapToReturn["shiptype"] = "Pilot Vessel";break;
            case 51:mapToReturn["shiptype"] = "Search and Rescue vessel";break;
            case 52:mapToReturn["shiptype"] = "Tug";break;
            case 53:mapToReturn["shiptype"] = "Port Tender";break;
            case 54:mapToReturn["shiptype"] = "Anti-pollution equipment";break;
            case 55:mapToReturn["shiptype"] = "Law Enforcement";break;
            case 56:;
            case 57:mapToReturn["shiptype"] = "Spare - Local Vessel";break;
            case 58:mapToReturn["shiptype"] = "Medical Transport";break;
            case 59:mapToReturn["shiptype"] = "Noncombatant ship according to RR Resolution No. 18";break;
            case 60:mapToReturn["shiptype"] = "Passenger, all ships of this type";break;
            case 61:mapToReturn["shiptype"] = "Passenger, Hazardous category A";break;
            case 62:mapToReturn["shiptype"] = "Passenger, Hazardous category B";break;
            case 63:mapToReturn["shiptype"] = "Passenger, Hazardous category C";break;
            case 64:mapToReturn["shiptype"] = "Passenger, Hazardous category D";break;
            case 65:;
            case 66:;
            case 67:;
            case 68:mapToReturn["shiptype"] = "Passenger, Reserved for future use";break;
            case 69:mapToReturn["shiptype"] = "Passenger, No additional information";break;
            case 70:mapToReturn["shiptype"] = "Cargo, all ships of this type";break;
            case 71:mapToReturn["shiptype"] = "Cargo, Hazardous category A";break;
            case 72:mapToReturn["shiptype"] = "Cargo, Hazardous category B";break;
            case 73:mapToReturn["shiptype"] = "Cargo, Hazardous category C";break;
            case 74:mapToReturn["shiptype"] = "Cargo, Hazardous category D";break;
            case 75:;
            case 76:;
            case 77:;
            case 78:mapToReturn["shiptype"] = "Cargo, Reserved for future use";break;
            case 79:mapToReturn["shiptype"] = "Cargo, No additional information";break;
            case 80:mapToReturn["shiptype"] = "Tanker, all ships of this type";break;
            case 81:mapToReturn["shiptype"] = "Tanker, Hazardous category A";break;
            case 82:mapToReturn["shiptype"] = "Tanker, Hazardous category B";break;
            case 83:mapToReturn["shiptype"] = "Tanker, Hazardous category C";break;
            case 84:mapToReturn["shiptype"] = "Tanker, Hazardous category D";break;
            case 85:;
            case 86:;
            case 87:;
            case 88:mapToReturn["shiptype"] = "Tanker, Reserved for future use";break;
            case 89:mapToReturn["shiptype"] = "Tanker, No additional information";break;
            case 90:mapToReturn["shiptype"] = "Other Type, all ships of this type";break;
            case 91:mapToReturn["shiptype"] = "Other Type, Hazardous category A";break;
            case 92:mapToReturn["shiptype"] = "Other Type, Hazardous category B";break;
            case 93:mapToReturn["shiptype"] = "Other Type, Hazardous category C";break;
            case 94:mapToReturn["shiptype"] = "Other Type, Hazardous category D";break;
            case 95:;
            case 96:;
            case 97:;
            case 98:mapToReturn["shiptype"] = "Other Type, Reserved for future use";break;
            case 99:mapToReturn["shiptype"] = "Other Type, no additional information";break;
        }

        std::bitset<9> bsToBow(vMessages[0].getBinaryMessage(), 240, 9);//to_bow
        mapToReturn["to_bow"] = std::to_string(bsToBow.to_ulong());

        std::bitset<9> bsToStern(vMessages[0].getBinaryMessage(), 249, 9);//to_stern
        mapToReturn["to_stern"] = std::to_string(bsToStern.to_ulong());

        std::bitset<6> bsToPort(vMessages[0].getBinaryMessage(), 258, 6);//to_port
        mapToReturn["to_port"] = std::to_string(bsToPort.to_ulong());

        std::bitset<6> bsToStarboard(vMessages[0].getBinaryMessage(), 264, 6);//to_starboard
        mapToReturn["to_starboard"] = std::to_string(bsToStarboard.to_ulong());

        mapToReturn["epfd"] = bitsToEPFD(270);

        mapToReturn["month"] = bitsToMonths(274);

        mapToReturn["day"] = bitsToDay(278);

        mapToReturn["hour"] = bitsToHour(283);

        mapToReturn["minute"] = bitsToMinute(288); 

        std::bitset<8> bsDraught(vMessages[0].getBinaryMessage(), 294, 8);//Draught
        mapToReturn["draught"] = std::to_string((double)bsDraught.to_ulong() / 10);

        mapToReturn["destination"] = bitsToString(302, 120);

        mapToReturn["dte"] = bitsToBool(422);//DTE
        if(mapToReturn["dte"] == "0")
            mapToReturn["dte"] = "Data terminal ready";
        else
            mapToReturn["dte"] = "Not ready (default)";

        //SPARE
    }
    else if(vMessages[0].getMessageType() == 18){
        
    }
    else if(vMessages[0].getMessageType() == 24){
        
    }

    return mapToReturn;
}