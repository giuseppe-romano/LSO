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
        4. [Giocare una partita](#play-game)
        5. [Terminazione del client](#stop-client)
4. [Specifiche tecniche](#tech-specs)
    1. [Il modulo shared](#shared-module)
        1. [La libreria logging](#shared-module-logging)
        2. [La libreria draw](#shared-module-draw)
        3. [La libreria serial](#shared-module-serial)
           1. [Le funzioni di serializzazione e deserializzazione della struct Game](#shared-module-serial-game)
           2. [Le funzioni di serializzazione e deserializzazione dell'azione di aggiunta al gioco di un giocatore](#shared-module-serial-add-cell)
           3. [Le funzioni di serializzazione e deserializzazione dell'azione di rimozione dal gioco di un giocatore](#shared-module-serial-remove-cell)
           4. [Le funzioni di serializzazione e deserializzazione della struct Player](#shared-module-serial-player)
           5. [Le funzioni di serializzazione e deserializzazione della richiesta di registrazione](#shared-module-serial-register-req)
           6. [Le funzioni di serializzazione e deserializzazione della richiesta di login](#shared-module-serial-login-req)
           7. [Le funzioni di serializzazione e deserializzazione della risposta di registrazione](#shared-module-serial-register-resp)
           8. [Le funzioni di serializzazione e deserializzazione della risposta di login](#shared-module-serial-login-resp)
           9. [Le funzioni di serializzazione e deserializzazione della richiesta di logout](#shared-module-serial-logout-req)
           10. [Le funzioni di serializzazione e deserializzazione della richiesta di movimento](#shared-module-serial-move-req)
           11. [Le funzioni di serializzazione e deserializzazione della risposta di movimento](#shared-module-serial-move-resp)
    2. [Il modulo server](#server-module)
    3. [Il modulo client](#client-module)

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
Il programma **Minefield Game** è un programma scritto completamente in linguaggio C su piattaforma Unix pertanto, per poter compilare il programma a partire dal codice sorgente, si richiede di istallare alcuni tools e librerie di base.

```sh
$ sudo apt-get install build-essential
```

Il pacchetto **build-essential** contiene tutti gli strumenti e le librerie di base per compilare pacchetti. Esso generalmente include i compilatori GCC/g++, librerie ed altri strumenti.

Inoltre occorre installare **git** client allo scopo di effettuare il cloning del repository sulla propria macchina locale. Per installare git eseguire il seguente comando:

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
Prima di poter avviare il server è necessario compilare il codice sorgente e generare l'eseguibile, per compilare seguire i passi descritti nella sezione [Compilazione del server.](#server-build)

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
In fase di avvio il server mostra un menù interattivo a sinistra e la matrice del campo minato con una sessione di gioco già avviata. Nella schermata del server, le mine sul campo minato sono visibili e marcate con una X di colore rosso.

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
Prima di poter avviare il client è necessario compilare il codice sorgente e generare l'eseguibile, per compilare seguire i passi descritti nella sezione [Compilazione del client.](#client-build)

Per avviare il client occorre specificare alcuni parametri di connessione sulla riga di comando:
```sh
$ cd myFolder/LSO/client
$ ./client --host SERVER_MACHINE --port SERVER_PORT --log client.log
```
Dove i parametri **host** e **port** indicano, rispettivamente, il nome macchina (o indirizzo IP) del server e la porta TCP sulla quale il server è in ascolto.
Il parametro **log** è opzionale, di default il logging viene effettuato nel file **client.log** nella medesima cartella. Ovviamente è possibile specificare qualsiasi percorso assoluto e/o relativo in cui voler scrivere il file di log.

*NOTA*: Se i parametri **host** e **port** sono errati (oppure il server è spento) allora il client termina con il seguente messaggio di errore:
```html
Unable to connect to the server: 'server:port'. Please check your parameters!
```

All'avvio del client viene mostrata la seguente schermata:

![](https://github.com/giuseppe-romano/LSO/raw/master/doc-images/client-main.jpg)

#### Registrarsi come nuovo utente <a name="register"></a>
Un utente, per accedere al gioco, deve registrarsi come giocatore. La fase di registrazione è molto semplice, è sufficiente fornire il nome utente, la password, il colore di gioco e il simbolo di gioco con cui l'utente verrà visualizzato sul campo minato.

Per effettuare la registrazione, quindi, dal menù principale digitare **1** e premere *INVIO*. Il sistema mostra una nuova schermata interattiva in cui viene chiesto di digitare alcune informazioni:

![](https://github.com/giuseppe-romano/LSO/raw/master/doc-images/client-register-username.jpg)

Digitare il proprio nome utente e premere *INVIO*
![](https://github.com/giuseppe-romano/LSO/raw/master/doc-images/client-register-password.jpg)

Digitare la propria password e premere *INVIO*.
![](https://github.com/giuseppe-romano/LSO/raw/master/doc-images/client-register-color.jpg)

Scegliere il colore con il quale essere visualizzato sul campo minato e premere *INVIO*.

**ATTENZIONE:** I colori possibili sono:
> * red 
> * green 
> * yellow 
> * blue
> * magenta
> * ciano
> * white

![](https://github.com/giuseppe-romano/LSO/raw/master/doc-images/client-register-symbol.jpg)

Digitare il proprio simbolo formato da un singolo carattere e premere *INVIO*.

Al termine, il sistema ritorna al menù principale mostrando un mesaggio di notifica sullo stato della registrazione appena effettuata. Se la registrazione ha avuto successo allora viene mostrata la seguente schermata:

![](https://github.com/giuseppe-romano/LSO/raw/master/doc-images/client-registration-done.jpg)

Se invece la registrazione è fallita magari perchè l'utente risulta già registrato allora il sistema mostra la seguente schermata:

![](https://github.com/giuseppe-romano/LSO/raw/master/doc-images/client-registration-failed.jpg)

#### Accedere al gioco <a name="login"></a>
Per accedere al gioco occorre essere registrati al sito, se non sei ancora registrato allora vai alla sezione [Registrarsi come nuovo utente.](#register)

Per effettuare l'accesso, dal menù principale digitare **2** e premere *INVIO*. Il sistema mostra una nuova schermata interattiva in cui viene chiesto di digitare il nome utente e la password forniti in fase di registrazione:

Digitare il nome utente e premere *INVIO*.
![](https://github.com/giuseppe-romano/LSO/raw/master/doc-images/client-login-username.jpg)

Digitare la password e premere *INVIO*.
![](https://github.com/giuseppe-romano/LSO/raw/master/doc-images/client-login-password.jpg)

Al termine, se la login ha avuto successo, il sistema mostra la schermata di gioco con il campo minato ed eventualmente gli altri giocatori connessi al gioco. 
Il sistema aggiunge il giocatore sulla prima colonna a sinistra in maniera casuale.

![](https://github.com/giuseppe-romano/LSO/raw/master/doc-images/client-login-done.jpg)

Si noti che le mine non sono visibili al giocatore ma sono soltanto visibili le posizioni degli altri giocatori.

Inoltre, se la login fallisce, il sistema ritorna al menù principale con un messaggio di notifica indicando che la username e/o password sono errati.

![](https://github.com/giuseppe-romano/LSO/raw/master/doc-images/client-login-failed.jpg)

#### Giocare una partita <a name="play-game"></a>
Per giocare una partita occorre dapprima effettuare la login, se non si è già loggati si prega di seguire la procedura descritta nella sezione [Accedere al gioco.](#login)

Una volta effettuata la login, il sistema mostra la schermata di gioco con il campo minato.

![](https://github.com/giuseppe-romano/LSO/raw/master/doc-images/client-login-done.jpg)

Dal menù di gioco è possibile scegliere una tra le quattro direzioni possibili (**S**, **N**, **E**, **O**).

> * Premere **1** e *INVIO* per spostarsi verso l'alto &uarr;
> * Premere **2** e *INVIO* per spostarsi verso destra &rarr;
> * Premere **3** e *INVIO* per spostarsi verso il basso &darr;
> * Premere **4** e *INVIO* per spostarsi verso sinistra &larr;

Ad ogni spostamento di un qualsiasi giocatore vengono notificati tutti i giocatori circa l'esito dello spostamento, infatti uno spostamento può determinare le seguenti condizioni:

> * Il giocatore si sposta con successo senza toccare alcun ostacolo e/o bomba. In tal caso tutti i giocatori vengono notificati con il messaggio: 
```html
The player 'G' (giuseppe) moved with success!
```

> * Il giocatore si sposta in una locazione già occupata da un altro giocatore. In tal caso l'azione diventa nulla e nessuno spostamento viene effettuato.

> * Il giocatore si sposta in una locazione esplosiva. In tal caso il giocatore che è saltato in aria viene riposizionato in maniera casuale sulla prima colonna e tutti i giocatori vengono notificati con il messaggio: 
```html
The player 'G' (giuseppe) hit a bomb. He blew up!
```

> * Il giocatore vince la partita raggiungendo una qualsiasi locazione dell'ultima colonna. In tal caso una nuova sessione di gioco viene generata in automatico dal server e tutti i giocatori vengono notificati con il messaggio: 
```html
The player 'G' (giuseppe) won. Game restarted!
```

#### Terminazione del client <a name="stop-client"></a>
Per terminare il client, dal menù principale, digitare **9** e premere *INVIO*.

## Specifiche tecniche <a name="tech-specs"></a>
In questa sezione vengono descritte le caratteristiche tecniche del programma, con particolare dettaglio sul protocollo di comunicazione utilizzato tra il server e i clients. 

### Il modulo shared <a name="shared-module"></a>
Il modulo **shared** è il modulo condiviso tra il server ed il client, esso contiene alcune librerie di utilità comuni atte a svolgere quei compiti supporto al core business dell'applicazione. Il modulo contiene tre librerie distinte e sono:

1. **logging**.h|c La libreria che gestisce la scrittura del file di log, essa fornisce alcune funzioni per il logging con diversi livelli di verbosità.

2. **draw**.h|c La libreria che gestisce la grafica del sistema, essa fornisce una serie di funzioni per disegnare l'header, il menù ed il contenuto principale del gioco ovvero la matrice del campo minato.

3. **serial**.h|c La libreria che implementa il protocollo di comunicazione tra server e client, essa implementa tutte le regole di serializzazione e deserializzazione dei messaggi scambiati sulla rete.

#### La libreria logging <a name="shared-module-logging"></a>
Questa libreria è scritta in maniera tale da mascherare tutte le operazioni di basso livello concernenti la scrittura del log in un apposito file. Essa utilizza un pattern di logging formato dal timestamp, il livello di verbosità, il contesto in cui l'operazione si è verificata ed il messaggio che descrive l'operazione. 
Il pattern è il seguente:
```c
"[TIMESTAMP][LOG LEVEL] - [CATEGORY] - MESSAGE"

esempi:
"[12-12-2020 23:59:59][INFO] - [MAIN] - Server started!"
"[12-12-2020 23:59:59][WARN] - [GAME] - User blew up!"
```

Il file header definisce le seguenti funzioni:

* > La funzione di inizializzazione del file di log, il parametro **filename** è il percorso assoluto (o relativo) al file di logging. La funzione apre il file in modalità *append* allo scopo di preservare il log pre-esistente.
```c
void initLogFile(char* filename);
```

* > La funzione di logging di livello **info**
```c
void info(char* category, char* message);
```

* > La funzione di logging di livello **warn**
```c
void warn(char* category, char* message);
```

* > La funzione di logging di livello **error**
```c
void error(char* category, char* message);
```

Oltre alle funzioni sopracitate, la libreria mette a disposizione anche funzione che fungono da scorciatoia nel senso che sono relative ad una ben specifica categoria. Come di seguito riportate, vediamo le funzioni relative alle categorie **MAIN**, **SERIAL**, **PLAYER**, **PROTOCOL**, **GAME** e **MENU**.

```c
void infoMain(char* message);
void warnMain(char* message);
void errorMain(char* message);
void infoSerial(char* message);
void infoPlayer(char* message);
void warnPlayer(char* message);
void infoProtocol(char* message);
void infoGame(char* message);
void warnGame(char* message);
void infoMenu(char* message);
void infoDraw(char* message);
```
Questa funzioni scorciatoia sono largamente utilizzate in tutto il programma (sia server che client).

#### La libreria draw <a name="shared-module-draw"></a>
Questa libreria implementa tutte le operazioni inerenti la grafica del sistema. Dal punto di vista del layout grafico, il sistema è suddiviso in diverse sezioni e sono il titolo, il menù interattivo, i messaggi di notifica e la matrice del campo minato. La libreria fornisce una serie di funzioni atte a svolgere questo compito.

* > La funzione di riposizionamento del cursore (input da tastiera ad una specifica coordinata)
```c
void setInteractiveCursorCoords(int x, int y);
```

* > La funzione che disegna il titolo sulla console del server
```c
void drawServerTitle();
```

* > La funzione che disegna il titolo sulla console del client
```c
void drawClientTitle();
```

* > La funzione che disegna la matrice del campo minato attingendo tutte le informazioni relative alla dimensione del campo, la posizione delle bombe e dei giocatori dalla struttura dati **Game**
```c
void drawMineField(Game *game);
```

* > La funzione che disegna le informazioni di una singolo giocatore.
```c
void drawPlayer(Player *player);
```

* > La funzione che disegna, in alto a destra del campo minato, il nome ed il simbolo del giocatore connesso al gioco.
```c
void drawConnectedPlayer(Cell *player);
```

* > La funzione che disegna il messaggio di notifica (in risposta ad un qualche evento)
```c
void printNotificationMessage(int notificationStatus, char *notificationMessage);
```

#### La libreria serial <a name="shared-module-serial"></a>
Questa libreria rappresenta il fondamento principale della comunicazione tra il server ed il client, essa implementa un vero e proprio protocollo di comunicazione e definisce tutte le regole di marshalling e unmarshalling da e verso le strutture dati utilizzate dal sistema.
In questa libreria vengono definite tutte le strutture dati ed il modo in cui queste devono essere serializzate allo scopo di poter inviare i messaggi via rete alla controparte.

Le strutture dati sono le seguenti:

* > La struct Player è una lista linkata contenente le informazioni dei giocatori.
```c
typedef struct player {
    char *username;
    char *password;
    char *symbol;
    char *color;

    struct player *next;
} Player;
```

* > La struct Cell è una lista linkata contenente le informazioni dei giocatori riguardanti il posizionamento sulla matrice di gioco.
```c
typedef struct cell {
    int x;
    int y;
    char *symbol;
    char *color;
    char *user;

    struct cell *next;
} Cell;
```

* > La struct Game è la struttura dati principale del gioco, essa infatti colleziona tutte le informazioni riguardanti la dimensione della matrice di gioco (rows e cols), il numero di bombe dispiegate su di essa ed infine la lista dei giocatori che stanno attualmente sfidando la sorte.
```c
typedef struct game {
    int rows;
    int cols;

    Cell *bombCells;

    Cell *playerCells;
} Game;
```

* > La struct AuthenticationRequest modella una richiesta di autenticazione, essa viene usata sia per richieste di registrazione che per richieste di login/logout.
```c
typedef struct authenticationRequest {
    char *username;
    char *password;
    char *symbol;
    char *color;
} AuthenticationRequest;
```

* > La struct AuthenticationResponse modella una risposta di autenticazione, essa viene usata sia per rispondere a richieste di registrazione sia per rispondere a richieste di login/logout.
```c
typedef struct authenticationResponse {
    int status;
    char *message;
} AuthenticationResponse;
```

* > La struct MovePlayerRequest modella una richiesta di movimento (una tra le quattro direzioni) che il client invia al server.
```c
typedef struct movePlayerRequest {
    Cell *player;
    int direction;
} MovePlayerRequest;
```

* > La struct MovePlayerResponse modella una risposta in conseguenza di una richiesta di movimento (una tra le quattro direzioni) che il client invia al server. Infatti i clients, per qualsiasi azione, inviano richieste al server ed è il server che dedice se concedere o meno tali richieste.
```c
typedef struct movePlayerResponse {
    Cell *player;
    int status;
} MovePlayerResponse;
```

Oltre a definire le strutture dati, la libreria fornisce una serie di funzioni per serializzare e deserializzare, rispettivamente, i messaggi da trasmettere ed i messaggi ricevuti.

##### Le funzioni di serializzazione e deserializzazione della struct Game. <a name="shared-module-serial-game"></a>

```c
char* serializeGame(Game *game);
Game* deserializeGame(char *string);
```

La funzione **serializeGame** prende in input la struttura dati Game e serializza le informazioni nel seguente formato:

```c
GAME<rows=20|cols=30|player={x=0,y=6,symbol=G,color=red,user=user1,}|player={x=12,y=16,symbol=H,color=blue,user=user2,}>
```

*ATTENZIONE:* La serializzazione ignora la lista delle bombe poichè queste non devono essere nota al client.

Nell'esempio di cui sopra, il gioco è formato da 20 righe, 30 colonne e vi sono 2 giocatori posizionati nelle rispettive coordinate.

La funzione **deserializeGame** invece effettua l'operazione inversa ovvero prende in input una stringa ed alloca una struct Game contenente tutte le informazioni contenute nel formato seriale.

##### Le funzioni di serializzazione e deserializzazione dell'azione di aggiunta al gioco di un giocatore. <a name="shared-module-serial-add-cell"></a>
```c
char* serializeAddedCell(Cell *cell);
Cell* deserializeAddedCell(char *string);
```

La funzione **serializeAddedCell** prende in input la struttura dati Cell e serializza le informazioni nel seguente formato:

```c
ADDED_CELL<user=user1|x=0|y=23|symbol=S|color=blue>
```

Nell'esempio di cui sopra, un nuovo giocatore **user1** è stato aggiunto al gioco corrente.

La funzione **deserializeAddedCell** invece effettua l'operazione inversa ovvero prende in input una stringa ed alloca una struct Cell contenente tutte le informazioni contenute nel formato seriale.

##### Le funzioni di serializzazione e deserializzazione dell'azione di rimozione dal gioco di un giocatore. <a name="shared-module-serial-remove-cell"></a>
```c
char* serializeRemovedCell(Cell *cell);
Cell* deserializeRemovedCell(char *string);
```

La funzione **serializeRemovedCell** prende in input la struttura dati Cell e serializza le informazioni nel seguente formato:

```c
REMOVED_CELL<user=user1|x=0|y=23|symbol=S|color=blue>
```

Nell'esempio di cui sopra, un nuovo giocatore **user1** è stato rimosso dal gioco corrente.

La funzione **deserializeRemovedCell** invece effettua l'operazione inversa ovvero prende in input una stringa ed alloca una struct Cell contenente tutte le informazioni contenute nel formato seriale.


##### Le funzioni di serializzazione e deserializzazione della struct Player. <a name="shared-module-serial-player"></a>
```c
char* serializePlayer(Player *player);
Player* deserializePlayer(char *string);
```

La funzione **serializePlayer** prende in input la struttura dati Player e serializza le informazioni nel seguente formato:

```c
user=user1|password=pwd|symbol=S|color=blue
```

La funzione **deserializePlayer** invece effettua l'operazione inversa ovvero prende in input una stringa ed alloca una struct Cell contenente tutte le informazioni contenute nel formato seriale.


##### Le funzioni di serializzazione e deserializzazione della richiesta di registrazione. <a name="shared-module-serial-register-req"></a>
```c
char* serializeRegisterRequest(char *username, char *password, char *color, char *symbol);
AuthenticationRequest* deserializeRegisterRequest(char *string);
```

La funzione **serializeRegisterRequest** prende in input il nome utente, la password, il colore e il simbolo; e serializza le informazioni nel seguente formato:

```c
REGISTER<user=user1|password=pwd|color=blue|symbol=S>
```

La funzione **deserializeRegisterRequest** invece effettua l'operazione inversa ovvero prende in input una stringa ed alloca una struct AuthenticationRequest contenente tutte le informazioni contenute nel formato seriale.

##### Le funzioni di serializzazione e deserializzazione della richiesta di login. <a name="shared-module-serial-login-req"></a>

```c
char* serializeLoginRequest(char *username, char *password);
AuthenticationRequest* deserializeLoginRequest(char *string);
```

La funzione **serializeLoginRequest** prende in input il nome utente e la password e serializza le informazioni nel seguente formato:

```c
LOGIN<user=user1|password=pwd>
```

La funzione **deserializeLoginRequest** invece effettua l'operazione inversa ovvero prende in input una stringa ed alloca una struct AuthenticationRequest contenente tutte le informazioni contenute nel formato seriale.


##### Le funzioni di serializzazione e deserializzazione della risposta di registrazione. <a name="shared-module-serial-register-resp"></a>

```c
char* serializeRegisterResponse(int status, char *message);
AuthenticationResponse* deserializeRegisterResponse(char *string);
```

La funzione **serializeRegisterResponse** prende in input lo stato della richiesta e il messaggio e serializza le informazioni nel seguente formato:

```c
REGISTER_RESPONSE<status=1|message=This is a message>
```

La funzione **deserializeRegisterResponse** invece effettua l'operazione inversa ovvero prende in input una stringa ed alloca una struct AuthenticationResponse contenente tutte le informazioni contenute nel formato seriale.


##### Le funzioni di serializzazione e deserializzazione della risposta di login. <a name="shared-module-serial-login-resp"></a>

```c
char* serializeLoginResponse(int status, char *message);
AuthenticationResponse* deserializeLoginResponse(char *string);
```

La funzione **serializeLoginResponse** prende in input lo stato della richiesta e il messaggio e serializza le informazioni nel seguente formato:

```c
LOGIN_RESPONSE<status=1|message=This is a message>
```

La funzione **deserializeLoginResponse** invece effettua l'operazione inversa ovvero prende in input una stringa ed alloca una struct AuthenticationResponse contenente tutte le informazioni contenute nel formato seriale.

##### Le funzioni di serializzazione e deserializzazione della richiesta di logout. <a name="shared-module-serial-logout-req"></a>

```c
char* serializeLogoutRequest(char *username);
AuthenticationRequest* deserializeLogoutRequest(char *string);
```

La funzione **serializeLogoutRequest** prende in input il nome utente e serializza le informazioni nel seguente formato:

```c
LOGOUT<user=user1>
```

La funzione **deserializeLogoutRequest** invece effettua l'operazione inversa ovvero prende in input una stringa ed alloca una struct AuthenticationRequest contenente tutte le informazioni contenute nel formato seriale.



##### Le funzioni di serializzazione e deserializzazione della richiesta di movimento (S, N, E, O). <a name="shared-module-serial-move-req"></a>

```c
char* serializeMovePlayerRequest(Cell *player, int direction);
MovePlayerRequest* deserializeMovePlayerRequest(char *string);
```

La funzione **serializeMovePlayerRequest** prende in input la struct Cell (il giocatore sul campo) e la direzione e serializza le informazioni nel seguente formato:

```c
MOVE_PLAYER<player={x=0,y=6,symbol=S,color=red,user=user1}|direction=10>
```

La funzione **deserializeMovePlayerRequest** invece effettua l'operazione inversa ovvero prende in input una stringa ed alloca una struct MovePlayerRequest contenente tutte le informazioni contenute nel formato seriale.


##### Le funzioni di serializzazione e deserializzazione della risposta di movimento (S, N, E, O). <a name="shared-module-serial-move-resp"></a>

```c
char* serializeMovePlayerResponse(Cell *player, int status);
MovePlayerResponse* deserializeMovePlayerResponse(char *string);
```

La funzione **serializeMovePlayerResponse** prende in input la struct Cell (il giocatore sul campo) e lo stato della richiesta evasa e serializza le informazioni nel seguente formato:

```c
MOVE_PLAYER_RESPONSE<player={x=0,y=6,symbol=S,color=red,user=user1}|status=1>
```

La funzione **deserializeMovePlayerResponse** invece effettua l'operazione inversa ovvero prende in input una stringa ed alloca una struct MovePlayerResponse contenente tutte le informazioni contenute nel formato seriale.



### Il modulo server <a name="server-module"></a>

### Il modulo client <a name="client-module"></a>

