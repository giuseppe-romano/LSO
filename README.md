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
    1. [Manuale d'uso del server](#user-guide-server)
        1. [Avvio del server](#start-server)
        2. [Avvia una nuova sessione di gioco](#start-new-session)
        3. [Visualizza la lista dei giocatori](#show-players-list)
        4. [Terminazione del server](#stop-server)
    2. [Manuale d'uso del client](#user-guide-client)
        1. [Avvio del client](#start-client)
        2. [Registrarsi come nuovo utente](#register)
        3. [Accedere al gioco](#login)
        4. [Terminazione del client](#stop-client)
4. [Specifiche tecniche](#tech-specs)

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
$ cd myFolder
$ git clone https://github.com/giuseppe-romano/LSO.git
```

A termine, nella cartella *myFolder* dovresti trovare una cartella **LSO** nella quale vi sono tutti i sorgenti. I sorgenti sono suddivisi in tre sotto-cartelle:
- **shared** contiene il codice condiviso tra il server ed il client; funzioni di logging, drawing etc etc
- **server** contiene il codice del programma server
- **client** contiene il codice del programma client

###  Compilazione del server <a name="server-build"></a>
Il codice sorgente del server risiede nella sotto-cartella **server**, per compilare quindi il server occorre eseguire i seguenti comandi:
```sh
$ cd myFolder/LSO/server
$ make
```
A termine, il make genera l'eseguibile **server** nella medesima cartella.

###  Compilazione del client <a name="client-build"></a>
Parimenti al server, il codice sorgente del client risiede nella sotto-cartella **client**, per compilare quindi il client occorre eseguire i seguenti comandi:
```sh
$ cd myFolder/LSO/client
$ make
```
A termine, il make genera l'eseguibile **client** nella medesima cartella.

## Manuale d'uso <a name="user-guide"></a>
In questa sezione vengono documentate tutte le funzionalità offerte dal gioco **MineField**, il gioco è suddiviso in due programmi distinti il server ed il client. Il manuale d'uso ha lo scopo di fornire indicazioni all'utente circa le funzionalità e gli scenari d'uso (principali ed alternativi/di errore).

### Manuale d'uso del server <a name="user-guide-server"></a>
Il programma server è il programma principale del gioco, esso offre una serie di servizi a tutti i client che vogliono connettersi e provare a sfidare la sorte giocando una partita sul campo minato. 
Il server è il detentore delle regole del gioco, ogni azione del client viene dapprima vagliata dal server per poi renderla effettiva ed eventualmente notificarla in broadcast agli altri clients collegati al gioco.

#### Avvio del server <a name="start-server"></a>
Prima di poter avviare il server è necessario compilare il codice sorgente e generare l'eseguibile, qualora la compilazione non fosse stata ancora effettuata si prega di eseguire la procedura descritta nella sezione [Compilazione del server.](#server-build)

Per avviare il server con i parametri di default, eseguire il seguente comando:
```sh
$ cd myFolder/LSO/server
$ ./server
```
Di default il server si mette in ascolto sulla porta **8000** e logga tutte le operazioni nel file **server.log**. 
Qualora si avesse la necessità di avviare il server su una porta diversa e/o voler scrivere il file log in un'altra locazione è possibile specificare questi parametri in fase di avvio, come ad esempio:
```sh
$ cd myFolder/LSO/server
$ ./server --port 1234 --log another/location/file.log
```
In fase di avvio il server mostra un menù interattivo a sinistra e la matrice del campo minato con una sessione già avviata. Nella schermata del server, le mine sul campo minato sono visibili e marcate con una X di colore rosso.

![](https://github.com/giuseppe-romano/LSO/raw/master/doc-images/server-main.jpg)

Il server genera sin da subito una sessione di gioco dispiegando diverse bombe in posizioni casuali ad eccezione della prima e dell'ultima colonna le quali sono, rispettivamente, la colonna in cui vengono posizionati inizialmente i giocatori e la colonna di arrivo dove il giocatore vince la partita.

#### Avvia una nuova sessione di gioco <a name="start-new-session"></a>
Per avviare una nuova sessione di gioco, digitare **1** e premere *INVIO*. 

Qualora vi fossero giocatori sul campo minato, questi verranno riposizionati sulla prima colonna in maniera casuale e potranno continuare a giocare nella nuova sessione di gioco.

#### Visualizza la lista dei giocatori <a name="show-players-list"></a>
Il server può in qualsiasi momento visualizzare la lista dei giocatori registrati e quelli che sono connessi alla sessione di gioco.
Per visualizzare la lista dei giocatori, digitare **2** e premere *INVIO*.

Il sistema mostra il seguente sotto menù :

![](https://github.com/giuseppe-romano/LSO/raw/master/doc-images/list-players-menu.jpg)

Da quì è possibile visualizzare la lista dei giocatori registrati e la lista dei giocatori connessi al gioco.

- Per visualizzare la lista dei giocatori registrati, digitare **1** e premere *INVIO*.

- Per visualizzare la lista dei giocatori connessi, digitare **2** e premere *INVIO*.

**NOTA:** Ogni giocatore viene visualizzato con un colore specifico, esso è il colore che l'utente ha scelto in fase di registrazione.

Per tornare al menù principale, digitare **9** e premere *INVIO*.

#### Terminazione del server <a name="stop-server"></a>
Per terminare il server, dal menù principale, digitare **9** e premere *INVIO*.

### Manuale d'uso del client <a name="user-guide-client"></a>
Il programma client è il programma utilizzato dai giocatori, esso fornisce diverse funzionalità per registrarsi al sito, effettuare il login/logout e di giocare la partita.

#### Avvio del client <a name="start-client"></a>
Prima di poter avviare il client è necessario compilare il codice sorgente e generare l'eseguibile, qualora la compilazione non fosse stata ancora effettuata si prega di eseguire la procedura descritta nella sezione [Compilazione del client.](#client-build)

Per avviare il client occorre specificare alcuni parametri di connessione sulla riga di comando:
```sh
$ cd myFolder/LSO/client
$ ./client --host SERVER_MACHINE --port SERVER_PORT --log client.log
```
Dove i parametri **host** e **port** indicano, rispettivamente, il nome macchine (o indirizzo IP) del server e la porta TCP sulla quale il server è in ascolto.
Il parametro **log** è opzionale, di default il log file viene scritto nel file **client.log** nella medesima cartella. Ovviamente è possibile specificare qualsiasi percorso assoluto e/o relativo in cui voler scrivere il file di log.

*NOTA*: Se i parametri **host** e **port** sono errati (oppure il server è spento) allora il client termina con il seguente messaggio di errore:
```html
Unable to connect to the server: 'server:port'. Please check your parameters!
```

All'avvio del client viene mostrata la seguente schermata:

![](https://github.com/giuseppe-romano/LSO/raw/master/doc-images/client-main.jpg)


#### Registrarsi come nuovo utente <a name="register"></a>

#### Accedere al gioco <a name="login"></a>

#### Terminazione del client <a name="stop-client"></a>

## Specifiche tecniche <a name="tech-specs"></a>
```c
char* substr = malloc(4);
strrncpy(substr, buff+10, 4);
```
