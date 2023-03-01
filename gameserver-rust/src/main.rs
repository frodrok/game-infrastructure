use log::{info, warn, error};

use std::net::{TcpListener, TcpStream, UdpSocket};

use env_logger::Env;

use std::io::BufReader;
use std::io::Write;
use std::io::BufRead;

use std::thread::JoinHandle;

fn handle_client(mut stream: TcpStream, tx: Sender<String>) {
    let buf_reader = BufReader::new(&mut stream);

    let http_request: Vec<_> = buf_reader
        .lines()
        .map(|result| result.unwrap())
        .take_while(|line| !line.is_empty())
        .collect();

    info!("request {:#?}", http_request);

    tx.send(http_request.join("nn")).expect("unable to send on channel");

    let response = "HTTP/1.1 200 OK\r\n\r\n";

    stream.write_all(response.as_bytes()).unwrap();
        
}

use std::thread;
use std::sync::mpsc::channel;
use std::sync::mpsc::{Sender, Receiver, TryRecvError, RecvError};
use std::sync::Arc;
use std::sync::Mutex;

use std::time::{Duration, Instant};

struct ServerState {
    player_names: Vec<String>
}

impl ServerState {
    fn new() -> Self {
        ServerState { player_names: vec!() }
    }

    fn player_connected(&mut self, username: String) {
        self.player_names.push(username);
    }

    fn get_player_names(&self) -> Vec<String> {
        self.player_names.clone()
    }
}

/* The only thing this thread does is send the current server state to the connected client
every 500ms
 */
fn start_player_handler_thread(socket: UdpSocket,
                               src: std::net::SocketAddr,
                               server_state: Arc<Mutex<ServerState>>,
                               messages: Receiver<String>
) -> JoinHandle<()> {
    
    let thread_id = thread::spawn(move || {

        let start = Instant::now();
        
        'player_handler: loop {

            match messages.try_recv() {
                Ok(message) => {
                    info!("player_handler_thread received message {}", message);
                    if message == "poison" {
                        info!("player_handler_thread terminating");
                        break 'player_handler;
                    }
                },
                Err(TryRecvError::Disconnected) => { warn!("messages channel disconnected"); },
                Err(TryRecvError::Empty) => {}
            }
            
            let first_username: String = match server_state.lock() {
                Ok(state) => String::from(&state.get_player_names()[0]),
                Err(e) => {
                    error!("couldn't lock state {:?}", e);
                    "empty".to_string()

                }
            };            

            info!("player_handler_thread sending state");
            match socket.send_to(first_username.as_bytes(), src) {
                Ok(v) => info!("sent {}", v),
                Err(e) => error!("could not send {:?}", e)                    
            }
            
            std::thread::sleep(Duration::from_millis(500));
        }
        
    });

    thread_id
}

use std::collections::HashMap;
use std::net::SocketAddr;

        
/* Iterate our open connections and delete connections that have not sent a keepalive in 5 seconds */
fn cull_connections(connections: Arc<Mutex<HashMap<String, (JoinHandle<()>, Sender<String>, Instant)>>>) {

    loop {
        info!("culling connections");

        if let Ok(mut connectiones) = connections.lock() {
            let mut to_delete = String::from("");
            let mut should_delete = false;
            for (ip, connection_info) in connectiones.iter_mut() {
                info!("ip {:?}, info {:?}", ip, connection_info);

                let last_updated = connection_info.2;

                info!("last updated {:?}", last_updated.elapsed());

                if last_updated.elapsed().as_millis() > 5000 {
                    info!("culling connection");

                    let sender_channel = &connection_info.1;

                    match sender_channel.send(String::from("poison")) {
                        Ok(r) => info!("sent poison pill"),
                        Err(e) => error!("could not send poison pill {:?}", e),
                    }
                    
                    should_delete = true;
                    to_delete = ip.clone();
                }
            }

            if should_delete {
                connectiones.remove(&to_delete);
                should_delete = false;
                to_delete = String::from("");
            }
        } else {
            warn!("could not lock connections");
        }

        std::thread::sleep(Duration::from_millis(1000));
    }
}

fn main() -> std::io::Result<()> {
    
    env_logger::Builder::from_env(Env::default().default_filter_or("info")).init();

    let bind_address = "0.0.0.0";
    let bind_port = 27015;

    info!("Listening on {}:{}", bind_address, bind_port);

    let socket = UdpSocket::bind(format!("{}:{}", bind_address, bind_port))?;

    let mut connection_count = 0;
    let max_connections = 4;

    let server_state: Arc<Mutex<ServerState>> = Arc::new(Mutex::new(ServerState::new()));

    let (tx, rx) = channel::<String>();

    let mut threads: Vec<String> = Vec::new();
    let mut connections: HashMap<String, &UdpSocket> = HashMap::new();

    let (incoming_msg_tx, incoming_msg_rx) = channel::<(String, UdpSocket, Option<SocketAddr>)>();

    /* String is the ip connected, JoinHandle is the thread_id, Sender is the channel we can send to to pass messages to the handler thread */
    let mut open_connections: Arc<Mutex<HashMap<String, (JoinHandle<()>, Sender<String>, Instant)>>> =
        Arc::new(
            Mutex::new(
                HashMap::new()
            )
        );

    /* Wut, several separate references are ok to pass to different threads, but not the same as that becomes a moved value */
    let data_ref_one = Arc::clone(&open_connections);
    let data_ref_two = Arc::clone(&open_connections);

    let connection_culler_thread = thread::spawn(move || cull_connections(data_ref_one));
    
    let message_receiver_thread = thread::spawn(move || {

        loop {
             /* non-blocking 
             match incoming_msg_rx.try_recv() {
                 Ok(value) => {
                     info!("receiver value {}", value);
                 },
                 Err(TryRecvError::Disconnected) => {
                     warn!("mrt disconnected");
                 },
                 Err(TryRecvError::Empty) => {
                     info!("ppc empty");
                 }
             } */

             /* blocking */
            match incoming_msg_rx.recv() {
                
                Ok((command, socket, socket_addr_opt)) => {
                    info!("receiver command {}", command);
                    
                    match command.as_str() {
                        com if com.contains("client:connect") => {
                            
                            info!("client wants to connect");

                            let splitt = command.split(":").collect::<Vec<&str>>();
                            let player_name = splitt[2];
                            info!("player_name {}", player_name);

                            server_state.lock().unwrap().player_connected(player_name.to_string());
                            
                            let (player_handler_tx, player_handler_rx) = channel::<String>();
                            
                            let player_handler_thread = start_player_handler_thread(socket,
                                                                                    socket_addr_opt.unwrap(),
                                                                                    server_state.clone(),
                                                                                    player_handler_rx);

                            /* Add the connection to our open connections map */
                            data_ref_two
                                .lock()
                                .unwrap()
                                .insert(socket_addr_opt.unwrap().to_string(),
                                        (player_handler_thread, player_handler_tx, Instant::now()));

                            
                        },
                        "client:keepalive" => {
                            /* Find our connection in open_connections and update the instant */
                            info!("received keepalive message");
                            if let Ok(mut connections) = data_ref_two.lock() {
                                let key_value = socket_addr_opt.unwrap().to_string();

                                (connections.get_mut(&key_value).unwrap()).2 = Instant::now();
                            } else {
                                warn!("could not lock connections map");
                            }
                        },
                        
                        
                        _ => { warn!("unknown command"); }
                    }
                },
                Err(e) => {
                    error!("error receiving {:?}", e);
                }
            }
            
        }
        
        
    });
    
    let socket_receiver_thread = thread::spawn(move || {
        
        info!("starting socket receiver thread");

        loop {
            
            let mut buf = [0; 512];

            if let Ok((amt, connection_from)) = socket.recv_from(&mut buf) {

                info!("received a packet with size {}", amt);
                let buf = &mut buf[..amt];
                
                let as_str = match std::str::from_utf8(&buf) {
                    Ok(v) => v.replace("\n", ""),
                    Err(e) => { error!("could not decode {:?} to string {:?}", buf, e);
                                "".to_string()
                    }
                };
                
                info!("as_str {}", as_str);
                
                incoming_msg_tx.send((as_str,
                                      socket.try_clone().expect("could not clone socket"),
                                      Some(connection_from))
                )
                    .expect("could not send message to internal threads");
                
            } else {
                incoming_msg_tx.send(("unknown_message".to_string(),
                                      socket.try_clone().expect("could not clone socket"),
                                      None)
                )
                                     .expect("could not send message to internal threads");
            }

        };            
        
    });

    socket_receiver_thread.join();    

    Ok(())

    
}
