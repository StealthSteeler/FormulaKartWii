#include "racemodeonline.h"
#include "racedata.h"
#include "raceinfo.h"

extern void VSRaceFinishFunc(RaceModeOnlineVs *racemode);
extern void* ptr_miscPacketHandler;
static u32 fixTimer[12] = {0};

const u32 gracePeriod = 60;

void RaceCompletionShare(RaceModeOnlineVs *racemode){
    //transmit own raceCompletion
    for(u32 hudid = 0; hudid < 2; hudid++){
        u32 pid = Racedata->main.scenarios[0].settings.hudPlayerIds[hudid];
        if (pid != 0xff){
            racemode->outPacket.raceCompletion[hudid] = (*(u32*)&(Raceinfo->players[pid]->raceCompletion)) >> 16;
        }
    }
    
    //process others raceCompletion
    for(u32 pid = 0; pid < Racedata->main.scenarios[0].playerCount; pid++){
        if(Racedata->main.scenarios[0].players[pid].playerType == PLAYER_REAL_ONLINE){  //only update laps from other people
            //get the corresponding player
            RaceinfoPlayer *player = Raceinfo->players[pid];

            //get the corresponding sent raceCompletion and the infered lap
            RaceHeader2VS* RH2 = GetRaceHeader2Buffer(ptr_miscPacketHandler, pid);
            u32 hudid = (((RH2->packedBits[5] >> 3) & 0xF) == pid) ? 0 : 1; //packedBits[5] contains a nibble indicating the playerid for player 1 of a packet (if its not equal that means the pid belongs to player 2 of a splitscreen client)
            u32 temp = (RH2->raceCompletion[hudid] << 16);
            float sentRaceCompletion = *(float*)(&temp); 

            fixTimer[pid] += 1;
            if (player->raceCompletion - sentRaceCompletion > 0.2) {
                if (fixTimer[pid] > gracePeriod) {
                    player->currentLap -= 1;
                }
            } else if (sentRaceCompletion - player->raceCompletion > 0.99) { 
                if (fixTimer[pid] > gracePeriod) {
                    player->currentLap += 1;
                }
            } else {
                fixTimer[pid] = 0;
            }
        }
    }

    //original function
    VSRaceFinishFunc(racemode);
}