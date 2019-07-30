/*
 * classification.h
 *
 *  Created on: 25 jun. 2019
 *      Author: ruawe
 */

#ifndef CLASSIFICATION_H_
#define CLASSIFICATION_H_

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define MAX_RECEIVERS                                   4

#define gesture_from_bottom_left_to_top_right           1
#define gesture_from_top_right_to_bottom_left           2
#define gesture_from_right_to_left                      3
#define gesture_from_left_to_right                      4

#define area_used                                       1
#define area_not_used                                   0
#define area_initialisation                             0


#define RECEIVER_ID_TOP_LEFT                            0
#define RECEIVER_ID_TOP_RIGHT                           1
#define RECEIVER_ID_BOTTOM_LEFT                         2
#define RECEIVER_ID_BOTTOM_RIGHT                        3

typedef struct{
    int id; // Receiver identifier (one of RECEIVER_ID_XXX)
    int time_valid_apperance; // 0 (invalid time), 1 (valid time)
    int time_valid_disapperance; // 0 (invalid time), 1 (valid time),
    uint64_t time_apperance;  // any (time value)
    uint64_t time_disapperance;  // any (time value)
}treceiver;

typedef struct{
    int cardinality; //set size
    int ids[MAX_RECEIVERS]; //id of receivers used
    treceiver receivers[MAX_RECEIVERS]; //array of receivers
}treceiver_time_info;

typedef struct{
    int used; //number of receivers
    treceiver_time_info receiver_time[MAX_RECEIVERS]; //array of time information of receivers
}tset;

extern void treceiversInit(treceiver *ptr, int num_receivers);

extern void treceiverTimeInfoInit(treceiver_time_info *ptr_time);

extern void tsetInit(tset *pts);

extern void tsetsArrayInit(tset *pts[], int array_size);

extern uint64_t absTimeReceivers(treceiver a, treceiver b);

extern int receiverInfoAdd(tset *pts, int recv_id, uint64_t time_app, uint64_t time_disapp);
/*
 * 1.- Search a pts->receiver with id == recv_id;
 * 2.- if found at position i, pts->receivers[i].valid = 1, pts->receivers[i].time = time, and return 1
 * 3.- If not found, do nothing and return 0
 */

extern void tsetCopy(tset *output_pts, tset *input_pts, int recv_id);

//extern void tsetsArrayCopy(tset *input_pts[], tset *output_pts[], int array_size_input);

extern tset **tsetsClassify(tset *input_pts[], tset *output_pts[], int bandgap, int array_size);

extern void tsetsFill(tset input_tsets[],treceiver receivers[],int array_size, uint64_t time_app, uint64_t time_disapp);

extern void *tsetRecognizeGestureThread(tset output_tsets[], int param_time_valid);

#ifdef __cplusplus
}
#endif

#endif /* CLASSIFICATION_H_ */
