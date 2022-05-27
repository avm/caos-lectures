

int main() {
    void *request = 0;
    pthread_cond_t request_ready;
    pthread_mutex_t rr_lock;


    void* my = NULL;
    pthread_mutex_lock(&rr_lock);
    while (!my) {
        if (request) {
            my = request;
            request = NULL;
        } else {
            pthread_cond_wait(&request_ready, &rr_lock);
        }
    }
    pthread_mutex_unlock(&rr_lock);
    // handle my request



    pthread_mutex_lock(&rr_lock);
    request = accept_request();
    pthread_mutex_unlock(&rr_lock);
    pthread_cond_broadcast(&request_ready);

}
