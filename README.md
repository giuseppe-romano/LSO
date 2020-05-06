# Minefield Game

![](https://github.com/giuseppe-romano/LSO/raw/master/doc-images/minefield.jpg)

## Indice
1. [Introduzione](#introduction)
2. [Istallazione](#installation)
    1. [Prerequisiti di sistema](#system-requirements)
    2. [Scaricare il codice sorgente](#source-code-download)
    3. [Compilazione del server](#server-build)
    4. [Compilazione del client](#client-build)
3. [Manuale d'uso](#user-guide)

## Introduzione <a name="introduction"></a>
Il server manterrà una rappresentazione dell'ambiente in cui verranno posizionati delle mine. L'ambiente sia rappresentato da una matrice in cui gli utenti si potranno spostare di un passo alla volta nelle quattro direzioni: **S**, **N**, **E**, **O**. 
Il server posizionerà nella matrice mine in posizioni random. 

Ogni utente, una volta connesso al server, verrà posizionato in una posizione random sulla prima colonna della matrice e dovrà raggiungere una qualunque posizione dell'ultima colonna.
La posizione delle mine sulla mappa sarà nascosta per l'utente, saranno invece visibili gli avversari. Dopo ogni passo l'utente riceverà l'informazione sull'effetto proprio movimento: 
- se lo spostamento porterà su di una mina l'utente verrà eliminato; 

- se lo spostamento porterà nella locazione di uno degli utenti, tale spostamento sarà nullo e l'utente rimarrà nella posizione precedente. 

Quando la locazione sarà raggiunta da uno dei giocatori il server notificherà agli utenti la fine della sessione e ne genererà una nuova.  Per accedere al servizio ogni utente dovrà prima registrarsi al sito indicando nickname e password.

Non c'è un limite a priori al numero di utenti che si possono collegare con il server.  Il client consentirà all'utente di collegarsi ad un server di comunicazione, indicando tramite riga di comando il nome o l'indirizzo IP di tale server e la porta da utilizzare. Una volta collegato ad un server l'utente potrà: registrarsi come nuovo utente o accedere al gioco come utente registrato. Il servizio permetterà all'utente di spostarsi di una posizione, disconnettersi, vedere la lista degli utenti collegati, vedere gli ostacoli incontrati e la posizione degli altri utenti.

Il server dovrà supportare tutte le funzionalità descritte nella sezione relativa al client. All'avvio del server, sarà possibile specificare tramite riga di comando la porta TCP sulla quale mettersi in ascolto. Il server sarà di tipo concorrente, ovvero è in grado di servire più clients simultanemente. Durante il suo regolare funzionamento, il server effettuerà logging delle attività principali in un file apposito. Ad esempio, memorizzando data e ora di connessione dei client, il loro nome simbolico (se disponibile, altrimenti l'indirizzoIP), data e ora del raggiungimento della posizione finale.

## Istallazione <a name="installation"></a>

### Prerequisiti di sistema <a name="system-requirements"></a>
Il programma **Minefield Game** è un programma scritto completamente in linguaggio C su piattaforma Unix pertanto, per poter compilare il programma a partire dal codice sorgente, nonchè per mandare in esecuzione il programma stesso, si richiede di istallare alcuni tools e librerie di base.

```sh
$ sudo apt-get install build-essential
```

Il pacchetto **build-essential** contiene tutti gli strumenti e le librerie di base per compilare pacchetti. Esso generalmente include i compilatori GCC/g++, librerie ed altri strumenti.

Inoltre occorre installare git client allo scopo di effettuare il cloning del repository sulla propria macchina locale. Per installare git eseguire il seguente comando:

```sh
$ sudo apt install git-all
```

Infine, per la compilazione del codice sorgente, viene utilizzato il make tool. Per installare make eseguire il seguente comando:

```sh
$ sudo apt-get install make
```

### Scaricare il codice sorgente <a name="source-code-download"></a>
Il codice sorgente è disponibile su github all'indirizzo [LSO](https://github.com/giuseppe-romano/LSO), pertanto è sufficiente clonare il repository sul proprio computer con il seguente comando:

```sh
$ cd myFavouriteFolder
$ git clone https://github.com/giuseppe-romano/LSO.git
```

A termine, nella cartella *myFavouriteFolder* dovresti trovare una cartella **LSO** nella quale vi sono tutti i sorgenti. I sorgenti sono suddivisi in tre sotto-cartelle:
- **shared** contiene il codice condiviso tra il server ed il client; funzioni di logging, drawing etc etc
- **server** contiene il codice del programma server
- **client** contiene il codice del programma client

###  Compilazione del server <a name="server-build"></a>
Il codice sorgente del server risiede nella sotto-cartella **server**, per compilare quindi il server occorre eseguire i seguenti comandi:
```sh
$ cd myFavouriteFolder/LSO/server
$ make
```
A termine, il make ha generato l'eseguibile **server** nella medesima cartella.

###  Compilazione del client <a name="client-build"></a>
Parimenti al server, il codice sorgente del client risiede nella sotto-cartella **client**, per compilare quindi il client occorre eseguire i seguenti comandi:
```sh
$ cd myFavouriteFolder/LSO/client
$ make
```
A termine, il make ha generato l'eseguibile **client** nella medesima cartella.

## Manuale d'uso <a name="user-guide"></a>

```c
char* substr = malloc(4);
strrncpy(substr, buff+10, 4);
```
