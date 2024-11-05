#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>
#include <stdbool.h>

#include "common.h"
#include "util.h"
#include "banking.h"
#include "pa2345.h"
#include "queue.h"
#include "lamport.h"

int n;

int main(int argc, char *argv[]) {
    if (argc < 3) return -1;
    
    bool mutexl = false;

    bool p_exists = false;
    for (uint32_t i = 0; i < argc; ++i){
        if (strcmp(argv[i], "-p") == 0) {
            if (i != argc - 1) {
                p_exists = true;
                n = stoi(argv[i + 1]);
            }
        }
        if (strcmp(argv[i], "--mutexl") == 0) {
            mutexl = true;
        }
    }

    if (!p_exists) {
        fprintf(stderr, "ERROR usage %s -p n\n", argv[0]);
        return -1;
    }
    
    open_pipes();
    FILE* events_log_file = fopen(events_log, "w");
    for (local_id id = 1; id <= n; ++id) {
        pid_t pid = fork();
        if (pid == -1) {
            fprintf(stderr, "Error in fork\n");
            return -1;
        }
        if (pid == 0) {
            // char path[15];
            // sprintf(path, "%dOUT.txt", id);
            // FILE* out = freopen(path, "w", stdout);
            // setvbuf(out, NULL, _IONBF, 0);
            // sprintf(path, "%dERR.txt", id);
            // FILE* err = freopen(path, "w", stderr);
            // setvbuf(err, NULL, _IONBF, 0);
            close_unused_pipes(id);
            int done = 0;
            inc_lamport_time();
            Message* msg = malloc(sizeof(Message));
            send_with_log(events_log_file, get_lamport_time(), id, STARTED, 0);

            for (local_id i = 1; i <= n; ++i){
                if (id != i) {
                    wait_receive((void *)(uint64_t)id, i, msg);
                    actualize_lamport_time(msg->s_header.s_local_time);
                    if (msg->s_header.s_type != STARTED) {
                        printf("<%d> Error got %d mes\n", getpid(), msg->s_header.s_type);
                        free(msg);
                        exit(-1);
                    }    
                }
            }
            fprintf(events_log_file, log_received_all_started_fmt, get_lamport_time(), id);
            printf(log_received_all_started_fmt, get_lamport_time(), id);

            // =======   WORK   ========== 

            char buffer[BUFSIZ];
            for (uint32_t i = 1; i <= id * 5; ++i) {
                sprintf(buffer, log_loop_operation_fmt, id, i, id*5);
                if (mutexl) {
                    done += request_cs((void *)(uint64_t)id);
                }

                print(buffer);
                
                if (mutexl) {
                    release_cs((void *)(uint64_t)id);
                }
            }

            // =========== END WORK ===========
            
            inc_lamport_time();
            send_with_log(events_log_file, get_lamport_time(), id, DONE, 0);
            //fprintf(stderr, "process %d got %d done\n", id, done);
            while (done < n - 1){
                int from = receive_any((void *)(uint64_t)id, msg);
                actualize_lamport_time(msg->s_header.s_local_time);
                switch(msg->s_header.s_type) {
                    case CS_REQUEST:
                        queue_push(create_node(msg->s_header.s_local_time, from));
                        inc_lamport_time();
                        MessageHeader head = {
                            .s_local_time = get_lamport_time(),
                            .s_magic = MESSAGE_MAGIC,
                            .s_payload_len = 0,
                            .s_type = CS_REPLY
                        };
                        msg->s_header = head;
                        send((void*)(uint64_t) id, from, msg);
                        break;
                    case CS_RELEASE:
                        free(queue_pop());
                        break;
                    case DONE:
                        //fprintf(stderr, "%d got done from %d\n", id, from);
                        ++done;
                        printf("%d: process %1d received DONE from %d\n", get_lamport_time(), id, from);
                        break;
                    default:
                        fprintf(stderr, "<%d> ERROR got %d mes from %d\n", getpid(), msg->s_header.s_type, from);
                        break;
                }
            }

            fprintf(events_log_file, log_received_all_done_fmt, get_lamport_time(), id);
            printf(log_received_all_done_fmt, get_lamport_time(), id);

            free(msg);    
            printf("%d: process %1d END his work\n", get_lamport_time(), id);
            close_used_pipes(id);
            exit(0);
        }
    }

    // FILE* out = freopen("0OUT.txt", "w", stdout);
    // setvbuf(out, NULL, _IONBF, 0);
    // FILE* err = freopen("0ERR.txt", "w", stderr);
    // setvbuf(err, NULL, _IONBF, 0);


    close_unused_pipes(0);
    Message* msg = malloc(sizeof(Message));

    for (local_id i = 0; i < n; ++i){
        receive_any((void *)(uint64_t)0, msg);
        actualize_lamport_time(msg->s_header.s_local_time);
        if (msg->s_header.s_type != STARTED) {
            printf("<%d> Error got %d mes\n", getpid(), msg->s_header.s_type);
            free(msg);
            return -1;
        }    
    }
    
    fprintf(events_log_file, log_received_all_started_fmt, get_lamport_time(), 0);
    printf(log_received_all_started_fmt, get_lamport_time(), 0);

    for (local_id i = 1; i <= n; ++i){
        wait_receive((void *)(uint64_t)0, i, msg);
        actualize_lamport_time(msg->s_header.s_local_time);
        if (msg->s_header.s_type != DONE) {
            printf("<%d> Error got %d mes\n", getpid(), msg->s_header.s_type);
            free(msg);
            return -1;
        }
    }

    fprintf(events_log_file, log_received_all_done_fmt, get_lamport_time(), 0);
    printf(log_received_all_done_fmt, get_lamport_time(), 0);

    free(msg);
    for (local_id i = 1; i < n + 1; ++i) wait(NULL);

    free_pipes();
    fclose(events_log_file);
    return 0;
}
