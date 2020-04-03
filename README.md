# SoalShiftSISOP20_modul3_E04

Soal Shift Sistem Operasi 2020\
Modul 2\
Kelompok E 04

## 1. Traizone & Pokezone
>Source code file: [Soal1](https://github.com/segara2410/SoalShiftSISOP20_modul3_E04/tree/master/soal1)

- Program Traizone merupakan `client` dan Pokezone merupakan `server`.
- Traizone memiliki dua mode, yaitu normal dan capture.
  - Mode normal untuk mencari pokemon dijalankan dengan `thread` tiap sepuluh detik.
- Pokezone memiliki tiga fitur
  - Fitur untuk mematikan traizone dan pokezone dijalankan dengan mengetikkan `shutdown`, menggunakan `fork-exec` di program main.
  - Fitur untuk merandom pokemon dijalankan menggunakan `thread` tiap satu detik.

## 2. Tap Game
>Source code file: [Soal2](https://github.com/segara2410/SoalShiftSISOP20_modul3_E04/tree/master/soal2)

- Program Tapplayer merupakan `client` dan Tapserver merupakan `server`.
- Tapplayer 
  - Mengirimkan command.
  - Mengirimkan Username dan Password untuk di login atau register.
  - Menerima pesan apakah player ini terkena hit atau tidak.
  - Ketika Game berjalan, responsive terhadap spasi dan mengirimkan tiap kali hit lawan.
- Tapserver
  - Menghandle banyak player dengan `thread`.
  - Membuat `akun.txt` untuk menyimpan akun yang telah diregister.
  - Melakukan pengecekan login, apakah akun ada atau tidak di `akun.txt`.
  - Menyediakan `queuing` untuk permainan agar dua pemain berpasangan.
  - Menerima hit dan mengirim pesan ke `client` yang terkena hit.
  - Mengirim pesan menang atau kalah ke `client`.

## 3. Kategorisasi File

>Source code file: [Soal3](https://github.com/segara2410/SoalShiftSISOP20_modul3_E04/tree/master/soal3)

- Kategorisasi menerima 3 argumen
  - `\*` untuk kategorisasi di folder dimana program tersebut berada.
  - `-f` untuk kategorisasi file yang ditentukan.
  - `-d` untuk kategorisasi di folder yang ditentukan.
- Pembuatan direktori untuk kategorisasi berdasarkan ekstensi tiap file.
- Dalam kategorisasi tiap file diproses dengan `thread` berbeda-beda.

## 4. Teka-Teki Batu Mulia

>Source code file: [Soal4](https://github.com/segara2410/SoalShiftSISOP20_modul3_E04/tree/master/soal4)

- 4a adalah program yang menghitung perkalian matriks dengan setiap perkaliannya dijalankan menggunakan `thread` kemudian memasukkan hasilnya ke shared memory.
- 4b menerima hasil dari 4a kemudian dilakukan operasi penambahan dari 1 ke n untuk tiap isi matriks mengguanakan `thread`.
- 4c adalah program untuk menampilkan jumlah folder dan file menggunakan `pipe` dan `fork-exec` command `ls | wc -l`.  