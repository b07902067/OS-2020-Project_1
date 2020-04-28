# 2020 OS Project 1 Report
### b07902067 資工二 郭宗頴

---
### Design
#### 此程式是在系統只有一個 CPU 的情況下才可以正確運作，當系統有多於一顆 CPU 時，運行的結果會變得無法確定。
#### 使用 `sched_idle` 暫停 process 和 `sched_other` 喚醒 process 以及 link-list 來實作四種排程方式，link-list的`head` 就是現在可以執行的 child process。
#### 四種方式都會在新的 child process 開始執行時，先以 `sched_idle` 暫停該 child process ，並進行 link-list 的串接以及順序調換，再使用 `sched_other` 繼續執行當前應該執行的工作，並且在 child  process 結束時，將 link-list 的 head 往後移，使用 `sched_other` 喚醒下一個程式。
- `FIFO` : 當新的 child process 被 `fork` 後，需要確認目前有沒有還在執行或等待的 process ，若沒有的話將 `head` 設定為新 fork 的 child process ，並開始執行。反之則把新 fork 的 child_process 接在 link-list 的最後。
- `RR` : 有新 fork 的 process 的動作如同 `FIFO` 。但當到達 time_quantum 時， 若當前在執行的 child process 還未結束，就把他接在 link-list的尾部，並把頭部往後移一個，輪到下一個繼續執行。
- `SJF` : 有新 fork 的 child_process 時，會根據剩餘時間長短排序，並插入 link_list 中，但不會和目前的 `head` 比較時間長短。剩餘時間越少越接近 `head`。
- `PSJF` : 有新的 child process 被 fork 之後，檢查剩餘時間，並和整個 link-list (包含 head) 比較，再插入 link-list。剩餘時間越少越接近 `head`。

#### 因為固定 link-list 的 `head` 為現在執行的 child process ，所以在 child process 結束時，只需要將 `head` 指標往後移動一個即可。
---
### System Call
### Kernel Version  - **Linux ubuntu 4.14.25**
#### system call 分為兩部分
- sys_proc_time: 獲得目前 kernel 的時間
    - 在 systemcall 裡使用標頭檔 `<linux/timekeeping.h>` 裡的 `getnstimeofday` 得到當前 kernel 的時間。
    - 因為 kernel space 和 user space 無法共用指標，在寫 system call 時，不能只是把 kernel 的資料 assign 給 user 的指標，要使用標頭檔 `<linux/uaccess.h>` 裡的 `copy_to_user` 複製給 user space 的 buffer。
        - 使用 `copy_to_user` 時，要在參數的地方加上 `__user`
    - 在 child process 新生和結束時各呼叫一次，就可以獲得開始和結束的時間。
- sys_proc_print: 將資料印出
    - 使用 printk
    - 在 child process exit 的時候呼叫
--- 
### 理論和實際
- 因為在 main process 多了串接 link-list 的動作，需要許多時間來完成，若 main process 和 child process 同時進行的話， child process 可能比在 main process 裡面紀錄的執行時間還早結束。
- main process 使用 `sched_setscheduler` 來 idle child process ，但在 child process 被 idle 之前，可能已經多執行了一段時間。
- 影響最大的原因：大概是因為 `SCHED_IDLE` 並非完全地暫停 process ，而是把優先權設定為非常低。當沒有FIFO或是OTHER的process在執行或等待時，IDLE的process還是會趁機執行。並且不是每個 process 在一開始執行就進入 `IDLE` ，所以還是會獲得執行的機會。



