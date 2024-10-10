調べたことを適宜メモする

## 参考資料
- https://beej.us/guide/bgnet/html/#sendrecv

# Tips
## atoiとstrtol
- atoi
  - https://man7.org/linux/man-pages/man3/atol.3.html
- strtol
  - https://man7.org/linux/man-pages/man3/strtol.3.html
- 以下のような理由で、strtolを利用した方が良い
  - https://www.gavo.t.u-tokyo.ac.jp/~dsk_saito/lecture/software2/misc/misc02.html
    - 整数以外の文字列が入力された時にエラーを検知できない
    - intの範囲を大きく超える整数の検出などもできない
- atoiに与える文字列が完全に管理できるなら、strtolより軽量（エラーチェックなし、10進数しか扱えない）なのでメリットもある

## 3ハンドシェイクとソケット関数との対応
| 3ウェイハンドシェイクのステップ	| ソケット関数 | 説明 |
| ---- | ---- | ---- |
| 1. SYN（接続要求） | connect() | サーバーにSYNを送り、3ウェイハンドシェイクを開始 |
| 2. SYN-ACK（応答） | accept() | サーバーが接続要求に応答（SYN-ACK）し、接続を確立 |
| 3. ACK（確認応答、接続完了） | accept() | クライアントからACKを受信し、通信準備が整う |

## 主要なシステムコール
### getaddrinfo
```c
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

int getaddrinfo(const char *node,     // e.g. "www.example.com" or IP
                const char *service,  // e.g. "http" or port number
                const struct addrinfo *hints,
                struct addrinfo **res);
```
- 関連情報を記載したaddrinfo構造体（hints）へのポインタを渡すことで、それを補完した完全な状態のaddrinfoのリンクドリストを作成してくれる
- エラーは```gai_strerror()```で拾う
- 使い終わったリンクドリストは```freeaddrinfo()```で解放する
- ```AI_PASSIVE```は、ローカルホストのアドレスを割り当てる設定

### socket
```c
#include <sys/types.h>
#include <sys/socket.h>

int socket(int domain, int type, int protocol); 
```
- ソケットディスクリプタを返す
- domain = PF_INET / PF_INET6
- type = SOCK_STREAM / SOCK_DGRAM
- 以下のように```getaddrinfo```で取得した情報を使って呼び出す
```c
getaddrinfo("www.example.com", "http", &hints, &res);
s = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
```

### bind
```c
#include <sys/types.h>
#include <sys/socket.h>

int bind(int sockfd, struct sockaddr *my_addr, int addrlen);
```
- ソケットをポートに関連付けする
- 1024以下のポートは予約されているので使わないこと
- ポートの再利用を許可するよう、以下のように設定できる
```c
int yes=1;
//char yes='1'; // Solaris people use this

// lose the pesky "Address already in use" error message
if (setsockopt(listener,SOL_SOCKET,SO_REUSEADDR,&yes,sizeof yes) == -1) {
    perror("setsockopt");
    exit(1);
} 
```

### connect
```c
#include <sys/types.h>
#include <sys/socket.h>

int connect(int sockfd, struct sockaddr *serv_addr, int addrlen); 
```
- 特定のIP、ポートに接続する
- ローカルのポートは気にしないので```bind```は呼ばない（カーネルが設定してくれる）

### listen
```c
int listen(int sockfd, int backlog); 
```
- 接続を待つ
- backlogは、着信キューで許可されるコネクション数
  - 着信してきたコネクションは、```accept()```するまでこのキューで待機することになる

### accept
```c
#include <sys/types.h>
#include <sys/socket.h>

int accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen);
```
- 接続を許可する
- 新しいソケットファイルディスクリプタが返される（こちらで```send/recv```する）

### send
```c
int send(int sockfd, const void *msg, int len, int flags); 
```
- 送信したいデータへのポインタとデータのバイト長を渡すことで、データを送信できる
- 実際に送信されたバイト長を返す
  - ```len```と一致しない場合は一部しか送信できていないことを意味する

### recv
```c
int recv(int sockfd, void *buf, int len, int flags);
```
- ```buf```に送られてきたデータを読み込む
- 0が返ってきた場合は、リモート側が接続を閉じたことを意味する

### close
```c
close(sockfd); 
```
- ソケットディスクリプタの接続を閉じる
- ```shutdown```を使うとより細かい制御(受信、送信、送受信の拒否)ができる
  - ```close```と異なり、ソケットディスクリプタを閉じるわけではないことに注意

# 用語集
