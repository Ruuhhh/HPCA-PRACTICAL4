#include &lt;stdio.h&gt;
#include &lt;stdlib.h&gt;
#include &lt;pthread.h&gt;
#include &lt;unistd.h&gt;
#define BUFFER_SIZE 5
#define NUM_ITEMS 10
int buffer1[BUFFER_SIZE], buffer2[BUFFER_SIZE];
int in1 = 0, out1 = 0, in2 = 0, out2 = 0;
int count1 = 0, count2 = 0;
pthread_mutex_t mutex1 = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex2 = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t full1 = PTHREAD_COND_INITIALIZER, empty1 =
PTHREAD_COND_INITIALIZER;
pthread_cond_t full2 = PTHREAD_COND_INITIALIZER, empty2 =
PTHREAD_COND_INITIALIZER;
void *stage1(void *arg) {
for (int i = 1; i &lt;= NUM_ITEMS; i++) {
pthread_mutex_lock(&amp;mutex1);
while (count1 == BUFFER_SIZE) pthread_cond_wait(&amp;empty1,
&amp;mutex1);
buffer1[in1] = i;
printf(&quot;Stage 1: Produced %d\n&quot;, i);
in1 = (in1 + 1) % BUFFER_SIZE;

count1++;
pthread_cond_signal(&amp;full1);
pthread_mutex_unlock(&amp;mutex1);
sleep(1);
}
return NULL;
}
void *stage2(void *arg) {
for (int i = 1; i &lt;= NUM_ITEMS; i++) {
pthread_mutex_lock(&amp;mutex1);
while (count1 == 0) pthread_cond_wait(&amp;full1, &amp;mutex1);
int val = buffer1[out1];
out1 = (out1 + 1) % BUFFER_SIZE;
count1--;
pthread_cond_signal(&amp;empty1);
pthread_mutex_unlock(&amp;mutex1);
int result = val * val;
pthread_mutex_lock(&amp;mutex2);
while (count2 == BUFFER_SIZE) pthread_cond_wait(&amp;empty2,
&amp;mutex2);
buffer2[in2] = result;
printf(&quot;Stage 2: Squared %d → %d\n&quot;, val, result);
in2 = (in2 + 1) % BUFFER_SIZE;
count2++;
pthread_cond_signal(&amp;full2);

pthread_mutex_unlock(&amp;mutex2);
sleep(1);
}
return NULL;
}
void *stage3(void *arg) {
for (int i = 1; i &lt;= NUM_ITEMS; i++) {
pthread_mutex_lock(&amp;mutex2);
while (count2 == 0) pthread_cond_wait(&amp;full2, &amp;mutex2);
int val = buffer2[out2];
out2 = (out2 + 1) % BUFFER_SIZE;
count2--;
pthread_cond_signal(&amp;empty2);
pthread_mutex_unlock(&amp;mutex2);

printf(&quot;Stage 3: Consumed %d\n&quot;, val);
sleep(1);
}
return NULL;
}

int main() {
pthread_t t1, t2, t3;
pthread_create(&amp;t1, NULL, stage1, NULL);

pthread_create(&amp;t2, NULL, stage2, NULL);
pthread_create(&amp;t3, NULL, stage3, NULL);
pthread_join(t1, NULL);
pthread_join(t2, NULL);
pthread_join(t3, NULL);
return 0;
}
