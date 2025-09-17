#include &lt;stdio.h&gt;
#include &lt;stdlib.h&gt;
#include &lt;unistd.h&gt;
#include &lt;pthread.h&gt;
#include &lt;mqueue.h&gt;
#include &lt;fcntl.h&gt;
#include &lt;string.h&gt;
#define NUM_ITEMS 10
#define QUEUE1 &quot;/mq_stage1&quot;
#define QUEUE2 &quot;/mq_stage2&quot;

void *stage1(void *arg) {
mqd_t q1 = mq_open(QUEUE1, O_WRONLY);
for (int i = 1; i &lt;= NUM_ITEMS; i++) {
mq_send(q1, (char*)&amp;i, sizeof(int), 0);
printf(&quot;Stage 1: Produced %d\n&quot;, i);
sleep(1);
}
mq_close(q1);
return NULL;
}
void *stage2(void *arg) {
mqd_t q1 = mq_open(QUEUE1, O_RDONLY);
mqd_t q2 = mq_open(QUEUE2, O_WRONLY);
for (int i = 1; i &lt;= NUM_ITEMS; i++) {
int val;
mq_receive(q1, (char*)&amp;val, sizeof(int), NULL);
int result = val * val;
mq_send(q2, (char*)&amp;result, sizeof(int), 0);
printf(&quot;Stage 2: Squared %d → %d\n&quot;, val, result);
sleep(1);
}
mq_close(q1);
mq_close(q2);
return NULL;

}

void *stage3(void *arg) {
mqd_t q2 = mq_open(QUEUE2, O_RDONLY);
for (int i = 1; i &lt;= NUM_ITEMS; i++) {
int val;
mq_receive(q2, (char*)&amp;val, sizeof(int), NULL);
printf(&quot;Stage 3: Consumed %d\n&quot;, val);
sleep(1);
}
mq_close(q2);
return NULL;
}

int main() {
struct mq_attr attr = {0, 10, sizeof(int), 0};
mq_unlink(QUEUE1);
mq_unlink(QUEUE2);
mq_open(QUEUE1, O_CREAT | O_RDWR, 0666, &amp;attr);
mq_open(QUEUE2, O_CREAT | O_RDWR, 0666, &amp;attr);

pthread_t t1, t2, t3;
pthread_create(&amp;t1, NULL, stage1, NULL);
pthread_create(&amp;t2, NULL, stage2, NULL);

pthread_create(&amp;t3, NULL, stage3, NULL);

pthread_join(t1, NULL);
pthread_join(t2, NULL);
pthread_join(t3, NULL);

mq_unlink(QUEUE1);
mq_unlink(QUEUE2);
return 0;
}
