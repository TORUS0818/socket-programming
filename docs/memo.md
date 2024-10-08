調べたことを適宜メモする

## 参考資料
- https://beej.us/guide/bgnet/html/#sendrecv

## Tips
### atoiとstrtol
- atoi
  - https://man7.org/linux/man-pages/man3/atol.3.html
- strtol
  - https://man7.org/linux/man-pages/man3/strtol.3.html
- 以下のような理由で、strtolを利用した方が良い
  - https://www.gavo.t.u-tokyo.ac.jp/~dsk_saito/lecture/software2/misc/misc02.html
    - 整数以外の文字列が入力された時にエラーを検知できない
    - intの範囲を大きく超える整数の検出などもできない
- atoiに与える文字列が完全に管理できるなら、strtolより軽量（エラーチェックなし、10進数しか扱えない）なのでメリットもある

### 3ハンドシェイクとソケット関数との対応
| 3ウェイハンドシェイクのステップ	| ソケット関数 | 説明 |
| ---- | ---- | ---- |
| 1. SYN（接続要求） | connect() | サーバーにSYNを送り、3ウェイハンドシェイクを開始 |
| 2. SYN-ACK（応答） | accept() | サーバーが接続要求に応答（SYN-ACK）し、接続を確立 |
| 3. ACK（確認応答、接続完了） | accept() | クライアントからACKを受信し、通信準備が整う |

## 用語集
