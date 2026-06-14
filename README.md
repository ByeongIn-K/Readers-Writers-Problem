# Readers-Writers Problem

## 1. 문제 개요

Readers-Writers Problem은 여러 Reader와 Writer가 공유 데이터에 접근하는 상황에서 발생하는 동기화 문제이다.

Reader는 데이터를 읽기만 수행하므로 여러 Reader가 동시에 공유 데이터에 접근할 수 있다.

반면 Writer는 데이터를 수정하므로 Writer가 공유 데이터에 접근하는 동안에는 다른 Reader 또는 Writer가 동시에 접근해서는 안 된다.

본 과제에서는 Readers-Writers Problem을 다음 두 가지 방식으로 구현하였다.

1. Reader Preference
2. Writer Preference

--------------------------------------------------

## 2. 구현 환경

- OS : Ubuntu 20.04 LTS
- Compiler : GCC 9.4.0
- Thread Library : POSIX Threads (pthread)

--------------------------------------------------

## 3. Reader Preference 버전

### 해결 방법

Reader Preference 방식은 Reader에게 우선권을 부여하는 방식이다.

첫 번째 Reader가 공유 자원을 잠그고 마지막 Reader가 공유 자원을 해제한다.

이를 통해 여러 Reader가 동시에 데이터를 읽을 수 있도록 하였다.

사용한 변수는 다음과 같다.

    pthread_mutex_t mutex;
    pthread_mutex_t wrt;
    int read_count = 0;

Reader는 다음 순서로 동작한다.

    mutex lock
    read_count++
    첫 번째 Reader이면 wrt lock
    mutex unlock

    Reading

    mutex lock
    read_count--
    마지막 Reader이면 wrt unlock
    mutex unlock

Writer는 다음 순서로 동작한다.

    wrt lock
    Writing
    wrt unlock

### 특징

- 여러 Reader가 동시에 접근 가능
- Reader의 응답 속도가 빠름
- Reader가 계속 들어오면 Writer가 대기할 수 있음
- Writer Starvation 발생 가능

--------------------------------------------------

## 4. Writer Preference 버전

### 해결 방법

Writer Preference 방식은 Writer에게 우선권을 부여하는 방식이다.

Writer가 대기 중인 경우 새로운 Reader의 진입을 제한하여 Writer가 먼저 실행될 수 있도록 구현하였다.

사용한 변수는 다음과 같다.

    pthread_mutex_t mutex;
    pthread_mutex_t wrt;
    pthread_mutex_t readTry;

    int read_count = 0;

Reader는 먼저 readTry를 통과해야 한다.

    readTry lock
    readTry unlock

이후 Reader Preference 방식과 동일하게 동작한다.

Writer는 다음 순서로 동작한다.

    readTry lock
    wrt lock

    Writing

    wrt unlock
    readTry unlock

이를 통해 Writer가 대기 중일 경우 새로운 Reader의 진입이 차단된다.

### 특징

- Writer가 우선적으로 실행됨
- Writer Starvation 방지
- Reader의 대기 시간이 증가할 수 있음
- Reader Preference보다 병렬성이 감소할 수 있음

--------------------------------------------------

## 5. 성능 측정 방법

성능 측정을 위해 별도의 성능 측정용 소스 파일을 작성하였다.

### 제출 파일

    reader_preference.c
    reader_preference_performance.c

    writer_preference.c
    writer_preference_performance.c

동작 확인용 소스 파일은 Reader와 Writer의 동작 과정을 출력하도록 구현하였다.

성능 측정용 소스 파일은 출력문을 제거하여 I/O 오버헤드가 결과에 영향을 주지 않도록 하였다.

Reader와 Writer는 각각 동일한 횟수의 작업을 수행하도록 설정하였다.

    #define ITER 100000

실행 시간은 프로그램 시작부터 모든 스레드 종료 시점까지를 측정하였다.

    clock_gettime(CLOCK_MONOTONIC, ...)

함수를 사용하여 총 실행 시간을 측정하였다.

또한 읽기 횟수(read_count)와 쓰기 횟수(write_count)는 여러 스레드가 동시에 접근하는 공유 변수이므로 mutex를 사용하여 Race Condition을 방지하였다.

--------------------------------------------------

## 6. 성능 측정 결과

| Method | Execution Time (sec) |
|----------|----------|
| Reader Preference | 측정 결과 |
| Writer Preference | 측정 결과 |

예시)

| Method | Execution Time (sec) |
|----------|----------|
| Reader Preference | 0.145321 |
| Writer Preference | 0.168754 |

--------------------------------------------------

## 7. 결과 분석

Reader Preference 방식은 여러 Reader가 동시에 공유 자원에 접근할 수 있기 때문에 높은 병렬성을 제공하였다.

Reader가 많은 환경에서는 Reader Preference 방식이 좋은 성능을 보였지만, Reader가 지속적으로 도착하는 경우 Writer가 오랫동안 실행되지 못하는 Writer Starvation 문제가 발생할 수 있었다.

Writer Preference 방식은 Writer가 대기 중인 경우 새로운 Reader의 진입을 제한하여 Writer가 우선적으로 실행될 수 있도록 하였다.

이를 통해 Writer Starvation 문제를 해결할 수 있었지만, Reader의 대기 시간이 증가하여 전체적인 병렬성이 감소하는 경향을 보였다.

실험 결과 Reader Preference 방식이 실행 시간 측면에서는 다소 유리한 경우가 많았지만, 시스템의 공정성(Fairness) 측면에서는 Writer Preference 방식이 더 적절한 해결 방법임을 확인할 수 있었다.

--------------------------------------------------

## 8. 결론 및 느낀점

이번 실습을 통해 Readers-Writers Problem에서 Reader와 Writer 사이의 동기화가 얼마나 중요한지 이해할 수 있었다.

Reader Preference 방식은 높은 병렬성을 제공하지만 Writer Starvation 문제가 발생할 수 있었고, Writer Preference 방식은 Writer Starvation을 방지하는 대신 Reader의 대기 시간이 증가하는 특징이 있었다.

이를 통해 동기화 문제에서는 단순히 실행 시간만 고려하는 것이 아니라 공정성(Fairness), 기아 상태(Starvation), 자원 활용 효율 등을 함께 고려해야 한다는 점을 확인할 수 있었다.

또한 동일한 문제라도 우선순위를 어디에 부여하느냐에 따라 시스템의 동작 방식과 성능이 크게 달라질 수 있음을 알 수 있었다.
