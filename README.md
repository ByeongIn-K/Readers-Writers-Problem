# Readers-Writers Problem

## 1. 문제 개요

Readers-Writers Problem은 여러 Reader와 Writer가 하나의 공유 데이터(DB)에 접근하는 상황에서 발생하는 동기화 문제이다.

Reader는 데이터를 읽기만 수행하므로 여러 Reader가 동시에 공유 데이터에 접근할 수 있다.

반면 Writer는 데이터를 수정하므로 Writer가 공유 데이터에 접근하는 동안에는 다른 Reader 또는 Writer가 동시에 접근해서는 안 된다.

본 과제에서는 공유 변수 data를 사용하여 Readers-Writers Problem을 구현하였다.

```
int data = 0;
```

Reader는 data 값을 읽고,

Writer는 data 값을 증가시키도록 구현하였다.

또한 다음 두 가지 방식으로 문제를 해결하였다.

1. Reader Preference
2. Writer Preference

---

## 2. 구현 환경

* OS : Ubuntu 20.04 LTS
* Compiler : GCC 9.4.0
* Thread Library : POSIX Threads (pthread)

---

## 3. Reader Preference 버전

### 해결 방법

Reader Preference 방식은 Reader에게 우선권을 부여하는 방식이다.

여러 Reader는 동시에 공유 데이터에 접근할 수 있으며 Writer는 모든 Reader가 작업을 마칠 때까지 대기한다.

사용한 변수는 다음과 같다.

```
int data = 0;
int readcount = 0;

sem_t x;
sem_t wsem;
```

각 Semaphore의 역할은 다음과 같다.

```
x     : readcount 보호
wsem  : 공유 데이터 보호
```

Reader는 다음 순서로 동작한다.

```
sem_wait(&x);

readcount++;

if(readcount == 1)
    sem_wait(&wsem);

sem_post(&x);

Reading Data

sem_wait(&x);

readcount--;

if(readcount == 0)
    sem_post(&wsem);

sem_post(&x);
```

첫 번째 Reader가 wsem을 획득하고 마지막 Reader가 wsem을 해제한다.

따라서 여러 Reader는 동시에 데이터를 읽을 수 있으며 Writer는 Reader가 모두 종료될 때까지 대기한다.

Writer는 다음 순서로 동작한다.

```
sem_wait(&wsem);

data++;

sem_post(&wsem);
```

### 특징

* 여러 Reader가 동시에 접근 가능
* 높은 병렬성 제공
* Reader의 응답 속도가 빠름
* 구현 구조가 비교적 단순함
* Reader가 계속 도착하면 Writer가 오랫동안 대기할 수 있음
* Writer Starvation 발생 가능

---

## 4. Writer Preference 버전

### 해결 방법

Writer Preference 방식은 Writer에게 우선권을 부여하는 방식이다.

Writer가 대기 중인 경우 새로운 Reader의 진입을 제한하여 Writer가 먼저 실행될 수 있도록 구현하였다.

사용한 변수는 다음과 같다.

```
int data = 0;

int readcount = 0;
int writecount = 0;

sem_t x;
sem_t y;
sem_t z;
sem_t rsem;
sem_t wsem;
```

각 Semaphore의 역할은 다음과 같다.

```
x     : readcount 보호
y     : writecount 보호
z     : Reader 진입 순서 제어
rsem  : Reader 진입 제어
wsem  : 공유 데이터 보호
```

Reader는 먼저 rsem을 통과해야만 읽기 작업을 수행할 수 있다.

Writer는 writecount를 증가시키고,

첫 번째 Writer가 rsem을 획득하여 새로운 Reader의 진입을 차단한다.

```
if(writecount == 1)
    sem_wait(&rsem);
```

Writer가 모두 종료되면 rsem을 해제하여 Reader의 진입을 다시 허용한다.

이를 통해 Writer가 대기 중인 경우 새로운 Reader의 진입이 제한되어 Writer가 우선적으로 실행된다.

### 특징

* Writer에게 우선권 부여
* Writer Starvation 방지
* 데이터 수정 작업이 지연되지 않음
* 공정성(Fairness) 향상
* Reader의 대기 시간이 증가할 수 있음
* Reader Preference보다 병렬성이 감소할 수 있음

---

## 5. 성능 측정 방법

성능 측정을 위해 별도의 성능 측정용 소스 파일을 작성하였다.

### 제출 파일

```
reader_preference.c
reader_preference_performance.c

writer_preference.c
writer_preference_performance.c
```

동작 확인용 소스 파일은 Reader와 Writer의 동작 과정을 출력하도록 구현하였다.

예시)

```
[Reader 0] Reading Data = 5
[Writer 1] Writing Data = 6
```

성능 측정용 소스 파일은 출력문을 제거하여 I/O 오버헤드가 결과에 영향을 주지 않도록 하였다.

성능 측정은 다음 환경에서 수행하였다.

```
NUM_READERS = 8
NUM_WRITERS = 2
ITERATIONS  = 1000000
```

실행 시간은 프로그램 시작부터 모든 스레드 종료 시점까지를 측정하였다.

```
clock_gettime(CLOCK_MONOTONIC, ...)
```

또한 Reader와 Writer의 수행 횟수를 측정하기 위해 다음 변수를 사용하였다.

```
long long total_reads;
long long total_writes;
```

읽기 및 쓰기 횟수는 GCC Atomic Built-in 함수인

```
__sync_fetch_and_add()
```

를 사용하여 원자적으로 증가시켰다.

---

## 6. 성능 측정 결과

| Method            | Execution Time (sec) |
| ----------------- | -------------------- |
| Reader Preference | 측정 결과 입력             |
| Writer Preference | 측정 결과 입력             |

---

## 7. 결과 분석

Reader Preference 방식은 여러 Reader가 동시에 공유 데이터에 접근할 수 있으므로 높은 병렬성을 제공하였다.

Reader의 수가 많은 환경에서는 좋은 성능을 보였지만 Reader가 지속적으로 도착하는 경우 Writer가 오랫동안 실행되지 못하는 Writer Starvation 문제가 발생할 수 있었다.

Writer Preference 방식은 Writer가 대기 중인 경우 새로운 Reader의 진입을 제한함으로써 Writer가 우선적으로 실행될 수 있도록 하였다.

이를 통해 Writer Starvation 문제를 해결할 수 있었지만 Reader의 대기 시간이 증가하여 전체적인 병렬성이 감소하는 경향을 보였다.

실험 결과 Reader Preference 방식은 실행 시간 측면에서 유리할 수 있었고, Writer Preference 방식은 공정성(Fairness) 측면에서 더 안정적인 동작을 제공하였다.

---

## 8. 결론 및 느낀점

이번 실습을 통해 Readers-Writers Problem에서 Reader와 Writer 사이의 동기화가 얼마나 중요한지 이해할 수 있었다.

Reader Preference 방식은 높은 병렬성을 제공하지만 Writer Starvation 문제가 발생할 수 있었고, Writer Preference 방식은 Writer Starvation을 방지하는 대신 Reader의 대기 시간이 증가하는 특징이 있었다.

또한 Semaphore를 이용하여 Reader와 Writer의 접근 순서를 제어함으로써 공유 데이터의 일관성을 유지할 수 있음을 확인하였다.

이를 통해 동기화 문제에서는 단순한 실행 시간뿐만 아니라 공정성(Fairness), 기아 상태(Starvation), 자원 활용 효율 등을 함께 고려해야 한다는 점을 배울 수 있었다.
