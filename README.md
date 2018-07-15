# Multi-Thread-Mutex-SharedMemory
CSE 344 HOMEWORK #5
MultiThread ödevi olan homework #5; 3 adet çiçekçi, 1 merkez ve olabildiğince
müşteriden oluşmaktadır. Ben ödev tasarladrken herbir çiçekçi, merkez ve herbir
müşteriye bir client atadım. Yani herbir işlem bir thread ile çalışmaktadır.
İlk olarak “data.dat” dosyasında çiçekçi ve clientlerı okuyorum sonra bunları
threadpool da threadlere atıyorum. Böylece threadlerim hazır oluyorlar. Merkez
içinde bir thread açıyorum. İşelmin bu adımı bitti.
Ikinci olarak herbir tread merkeze istek yolluyor. Fakat istek yollarken,
herbiri örneğin merkezi aradığında telefonu kilitleyip, talebini yolluyor sonra
telefonu kapattığında diğer thread telefon açabiliyor.
Üçüncü olarak central en yakın çiçekçiyi buluyor ve çiçekçiye client'ın talebini
atıyor. Daha sonra çiçekçi talebe cevap veriyor ve ekrana basıyor.
En son gerçekleşen tüm işlemlerin özeti ekrana basılıyor.
DENİZ BABAT
131044013
