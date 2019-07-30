/*
 * classification.c
 *
 *  Created on: 25 jun. 2019
 *      Author: ruawe
 */
#include <classification.h>
#include <stdint.h>
#include <stdlib.h>
#include <ti/drivers/Timer.h>
#include <pthread.h>

static SemaphoreP_Handle timerSem;

SemaphoreP_Params semParams;
Timer_Params timerParams;
Timer_Handle timer;


void treceiversInit(treceiver *ptr, int num_receivers) {
    int i;
    for (i=0; i < num_receivers; i++) {
        ptr->id = i;
        ptr->time_apperance = 0;
        ptr->time_disapperance = 0;
        ptr->time_valid_apperance = 0;
        ptr->time_valid_disapperance = 0;
    }
}

void treceiverTimeInfoInit(treceiver_time_info *ptr_time) {
    int i;
    ptr_time->cardinality = 0;
    for(i=0; i<MAX_RECEIVERS; i++){
        ptr_time->ids[i] = i;
        ptr_time->receivers[i].id = i;
        ptr_time->receivers[i].time_apperance = 0;
        ptr_time->receivers[i].time_disapperance = 0;
        ptr_time->receivers[i].time_valid_apperance = 0;
        ptr_time->receivers[i].time_valid_disapperance = 0;
    }
}

void tsetInit(tset *pts) {
    int i;
    int j;
    pts->used = 0;
    for (i=0; i<MAX_RECEIVERS; i++) {
        pts->receiver_time[i].cardinality = 0;
        for(j=0; j<MAX_RECEIVERS; j++){
            pts->receiver_time[i].ids[j] = j;
            pts->receiver_time[i].receivers[j].id = j;
            pts->receiver_time[i].receivers[j].time_apperance = 0;
            pts->receiver_time[i].receivers[j].time_disapperance = 0;
            pts->receiver_time[i].receivers[j].time_valid_apperance = 0;
            pts->receiver_time[i].receivers[j].time_valid_disapperance = 0;
        }
    }
}

void tsetsArrayInit(tset *pts[], int array_size) {
    int i;
    for (i=0; i<array_size; i++) {
        tsetInit(pts[i]);
    }
}

uint64_t absTimeReceivers(treceiver a, treceiver b){
    uint64_t abs_apperance;
    uint64_t abs_disapperance;
    uint64_t abs_return;
    abs_apperance = abs(a.time_apperance-b.time_apperance);
    abs_disapperance = abs(a.time_disapperance-b.time_disapperance);
    if((a.time_valid_apperance == 1)&&(b.time_valid_apperance == 1)){
        abs_return = abs_apperance;
    }else if((a.time_valid_disapperance == 1)&&(b.time_valid_disapperance == 1)){
        abs_return = abs_disapperance;
    }
    return abs_return;
}

int receiverInfoAdd(tset *pts, int recv_id, uint64_t time_app, uint64_t time_disapp){
    int i;
    int j;
    int a;
    for(i=0; i<MAX_RECEIVERS; i++){
        for(j=0; j<MAX_RECEIVERS; j++){
            pts->receiver_time[i].cardinality = pts->receiver_time[i].cardinality + 1;
            if(pts->receiver_time[i].receivers[j].id == recv_id){
                pts->receiver_time[i].ids[j] = recv_id;
                pts->receiver_time[i].receivers[j].time_apperance = time_app;
                pts->receiver_time[i].receivers[j].time_valid_apperance = 1;
                pts->receiver_time[i].receivers[j].time_disapperance = time_disapp;
                pts->receiver_time[i].receivers[j].time_valid_disapperance = 1;
                a = 1;
            }else{
                a = 0;
            }
        }
    }
    return a;
}

void tsetCopy(tset *output_pts, tset *input_pts, int recv_id){
    int i;
    int j;
    uint64_t time_app;
    uint64_t time_disapp;
    output_pts->used = input_pts->used;
    for(i=0; i<MAX_RECEIVERS; i++){
        for(j=0; j<MAX_RECEIVERS; j++){
            time_app = input_pts->receiver_time[i].receivers[j].time_apperance;
            time_disapp = input_pts->receiver_time[i].receivers[j].time_disapperance;
            receiverInfoAdd(output_pts, recv_id, time_app, time_disapp);
        }
    }
}

/*void tsetsArrayCopy(tset *input_pts[], tset *output_pts[], int array_size_input){
    int i;
    for(i=0; i<array_size_input; i++){
        tsetCopy(input_pts[i], output_pts[i]);
    }
}*/

tset **tsetsClassify(tset *input_pts[], tset *output_pts[], int bandgap, int array_size){
    int i;
    int j;
    int k;
    tset **pOut;
    int abs;
    int id;
    for(i=0; i<array_size; i++){
        for(j=0; j<array_size; j++){
            for(k=0; k<array_size; k++)
                abs = absTimeReceivers(input_pts[i]->receiver_time[j].receivers[k],input_pts[i]->receiver_time[j].receivers[k+1]);
                if(abs <= bandgap){
                    id = input_pts[i]->receiver_time[j].receivers[k+1].id;
                    tsetCopy(output_pts[i], input_pts[i], id);
                }
        }
    }
    pOut = output_pts;
    return pOut;
}

void tsetsFill(tset input_tsets[],treceiver receivers[],int array_size, uint64_t time_app, uint64_t time_disapp){
    int i;
    int j;
    int k;
    for(i=0; i<array_size; i++){
        input_tsets[i].used = MAX_RECEIVERS;
        for(j=0; j<array_size; j++){
            input_tsets[i].receiver_time[j].cardinality = 1;
            input_tsets[i].receiver_time[j].ids[0] = j;
            for(k=0; k<array_size; k++){
                input_tsets[i].receiver_time[j].receivers[k].time_apperance = time_app;
                input_tsets[i].receiver_time[j].receivers[k].time_valid_apperance = 1;
                input_tsets[i].receiver_time[j].receivers[k].time_disapperance = time_disapp;
                input_tsets[i].receiver_time[j].receivers[k].time_valid_disapperance = 1;
            }
        }
}

void *tsetRecognizeGestureThread(tset output_tsets[], int param_time_valid){

    while(1){

    }

}

main() {

    int i;
    int j;
    int k;
    int gesture_recognize;
    int bandgap;

    tset input_tsets[MAX_RECEIVERS];
    tset *input_pts[MAX_RECEIVERS];

    tset output_tsets[MAX_RECEIVERS];
    tset *output_pts[MAX_RECEIVERS];

    treceiver receivers[MAX_RECEIVERS];
    treceiver *ptr = receivers;


    for(i=0;i<MAX_RECEIVERS; i++){
         input_pts[i] = &input_tsets[i];
    }
    for(j=0;j<MAX_RECEIVERS; j++){
         output_pts[j] = &output_tsets[j];
    }

    for(k=0; k<MAX_RECEIVERS; k++){
        if (ptr[k].time_valid==0) {
            // Add time capture to receivers
            ptr[k].time=CAPTURE_TIME;//CAPTURE_TIME is the time data from this receiver
        }
        if (ptr[k].time_valid==1) {
            // Remove time capture to receivers
            ptr[k].time=0;
        }
    }



    treceiversInit(ptr, MAX_RECEIVERS);
    tsetsArrayInit(input_pts, MAX_RECEIVERS);
    tsetsArrayInit(output_pts, MAX_RECEIVERS);
    tsetsFill(input_tsets, receivers, MAX_RECEIVERS); // Add all receivers time info, each in a separate group (all groups with cardinality = 1)
    tsetsArrayCopy(output_pts, input_pts, MAX_RECEIVERS); // Copy 2nd onto 1st

    gesture_recognize = 0;
    bandgap = 10;
    do {
        tsetsArrayCopy(output_pts, input_pts, MAX_RECEIVERS); // Copy 2nd onto 1st
        tsetsClassify(input_pts, output_pts, bandgap, MAX_RECEIVERS);
        gesture_recognize = tsetRecognizeGesture(output_tsets);
        bandgap = bandgap + 10;
    } while (!gesture_recognize || (/*output_tsets*/input_tsets[0].cardinality == MAX_RECEIVERS));
}



