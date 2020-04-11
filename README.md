# SoalShiftSISOP20_modul3_E04

Soal Shift Sistem Operasi 2020\
Modul 2\
Kelompok E 04

## 1. Traizone & Pokezone
>Source code file: [Soal1](https://github.com/segara2410/SoalShiftSISOP20_modul3_E04/tree/master/soal1)

- Program Traizone merupakan dan Pokezone merupakan dua program yang berjalan dengan IPC.
- Traizone memiliki dua mode, yaitu normal dan capture.
  - Mode normal untuk mencari pokemon dijalankan dengan `thread` tiap sepuluh detik dari `shared memory`, membuka pokedex untuk mengelola pokemon, membuka shop untuk membeli item dari `shared memory`.
  - Mode Capture untuk Menangkap dan menggunakan item.
  - Ketika pokemon ditangkap, pokemon tersebut akan memiliki `thread` sendiri untuk meng-handle APnya.
  - Item lullaby powder dihandle dengan menggunakan `thread`
- Pokezone memiliki tiga fitur
  - Fitur untuk mematikan traizone dan pokezone dijalankan dengan mengetikkan `shutdown`, menggunakan `fork-exec` perintah `kill` dan `killall` di program main.
  - Fitur untuk merandom pokemon dijalankan menggunakan `thread` tiap satu detik kemudian di share menggunakan `shared memory`.
  - Fitur untuk merestock Item setiap sepuluh detik dengan `thread` kemudian di share menggunakan `shared memory`.

## 2. Tap Game
>Source code file: [Soal2](https://github.com/segara2410/SoalShiftSISOP20_modul3_E04/tree/master/soal2)

- Program Tapplayer merupakan `client` dan Tapserver merupakan `server`.
- Tapplayer 
  - Mengirimkan command login, logout, register, dan find.
  - Mengirimkan Username dan Password untuk di login atau register.
  - Menerima pesan apakah player ini terkena hit atau tidak.
  - Ketika Game berjalan, responsive terhadap spasi menggunakan `thread` serta `termios` untuk menghandle tiap spasi dan mengirimkan pesan tiap kali hit lawan.
- Tapserver
  - Menghandle banyak player dengan `thread` dan id dari `thread` tersebut akan menjadi id dari client juga.
  - Membuat `akun.txt` untuk menyimpan akun yang telah diregister menggunakan `fputs()`.
  - Melakukan pengecekan login, apakah akun ada atau tidak di `akun.txt` dengan `fgets()` tiap line dari akun.txt.
  - Menyediakan `queu1ing` untuk permainan agar dua pemain berpasangan dengan cara mencocokkan id yang telah diterima dari parameter untuk pembuatan server.
  - Menerima hit dan mengirim pesan ke `client` yang terkena hit.
  - Mengirim pesan menang atau kalah ke `client`.

## 3. Kategorisasi File

>Source code file: [Soal3](https://github.com/segara2410/SoalShiftSISOP20_modul3_E04/tree/master/soal3)

- Kategorisasi menerima 3 argumen
  - `\*` untuk kategorisasi di folder dimana program tersebut berada.
  - `-f` untuk kategorisasi file yang ditentukan.
  - `-d` untuk kategorisasi di folder yang ditentukan.
- Pembuatan direktori untuk kategorisasi berdasarkan ekstensi tiap file. Pengecekan ekstensi dan nama file menggunakan `strrchr()`
- Dalam kategorisasi tiap file diproses dengan `thread` berbeda-beda.
- Pemindahan file dilakukan dengan cara memmbaca file menggunakan `fgetc()` kemudian memasukkan ke file lain dengan nama yang sama dan folder yang berbeda menggunakan `fputc()`.

## 4. Teka-Teki Batu Mulia

>Source code file: [Soal4](https://github.com/segara2410/SoalShiftSISOP20_modul3_E04/tree/master/soal4)

- 4a adalah program yang menghitung perkalian matriks dengan setiap perkaliannya dijalankan menggunakan `thread` kemudian memasukkan hasilnya ke `shared memory`.
- 4b menerima hasil dari 4a dengan cara membaca `shared memory` kemudian dilakukan operasi penambahan dari 1 ke n untuk tiap isi matriks mengguanakan `thread`.
- 4c adalah program untuk menampilkan jumlah folder dan file menggunakan `pipe` dan `fork-exec` command `ls | wc -l`.  