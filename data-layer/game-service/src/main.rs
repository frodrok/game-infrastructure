use log::{info, warn, error};
use postgres::{Client, NoTls};

//use core::fmt::Error;
use postgres::Error;

use tokio;

use std::env;

mod errors;

use crate::errors::app_errors;
use crate::errors::app_errors::*;

use rocket_db_pools::{Connection, Database};

use std::collections::HashMap;

use std::sync::mpsc;

#[macro_use] extern crate rocket;

#[get("/")]
fn index() -> &'static str {
    "Hello, world!"
}

use rocket::serde::Deserialize;
use rocket::serde::Serialize;
use rocket::serde::json;
use rocket::serde::json::Json;


#[derive(Deserialize, Serialize, Debug)]
struct LoginRequest {
    username: String,
    password: String
}

#[derive(Serialize, Debug)]
struct LoginResponse {
    username: String,
    user_token: String
}

use rocket::http::Status;
use rocket::response::status;
use chrono::Local;
use sha2::{Sha512, Digest};
 

fn generate_user_token(username: &String,
                       password: &String) -> String {

    let current_time = Local::now();
    let mut hasher = Sha512::default();
    let strong_string = username.to_owned() + password + &current_time.to_string();
    hasher.update(strong_string.as_bytes());
    let res = format!("{:x}", hasher.finalize());
    res
}

#[post("/account/login", format="json", data="<login_info>")]
async fn login(mut db: Connection<PostgresDB>,
               login_info: Json<LoginRequest>) -> status::Custom<String> {
    info!("login called {:?}", login_info);

    let get_user_result = db.query(db::queries::GET_USER, &[&login_info.username]).await;

    match get_user_result {
        Ok(db_res) => {
            
            info!("rr: {:?}", db_res);

            let username = &login_info.username;

            if db_res.len() < 1 {
                status::Custom(Status::NotFound, String::from("not_found"))
            } else {

                let password_hashed = hash_password(&login_info.password);

                let mut db_username: String = String::from("");
                let mut db_password: String = String::from("");

                for row in db_res {
                    db_username = row.get::<&str, String>("username");
                    db_password = row.get::<&str, String>("password");
                }

                let success = db_password == password_hashed;

                info!("username {:?}, password {:?} password_hashed {:?} success {:?}", db_username, db_password, password_hashed, success);
                
                let user_token = generate_user_token(&login_info.username, &login_info.password);

                let set_token_result = db.query(db::queries::SET_USER_TOKEN, &[&user_token, &username]).await;

                match set_token_result {
                    Ok(v) => {
                        let json_string: String = json::to_string(&LoginResponse {
                            username: username.to_owned(),
                            user_token: user_token.to_string()
                        }).unwrap();
                        
                        status::Custom(Status::Ok, String::from(json_string))
                    },
                    Err(e) => {
                        status::Custom(Status::new(500), String::from("couldn't set token"))
                    }
                }
                
                
                
            }
        },
        Err(e) => {
            let err_status = Status::new(500);
            error!("account_login error {:?}", e);
            status::Custom(err_status, String::from("sorry"))
        }
    }
}

fn hash_password(password: &String) -> String{
    let mut hasher = Sha512::default();
    hasher.update(password.to_owned().as_bytes());
    let res = format!("{:x}", hasher.finalize());
    res
}

#[post("/account/register", format="json", data="<login_info>")]
async fn register(mut db: Connection<PostgresDB>,
                  login_info: Json<LoginRequest>) -> status::Custom<String> {
    
    info!("register called {:?}", login_info);

    let sha512_hash = hash_password(&login_info.password);

    let user_result = db.query(db::queries::INSERT_USER, &[&login_info.username, &sha512_hash]).await;

    match user_result {
        Ok(u) => {
            info!("created user {:?}", u);

            status::Custom(Status::Ok, String::from("okay"))
        }
        Err(e) => {
            let err_status = Status::new(500);

            // TODO: If duplicate, return 409 Conflict
            error!("account_register error {:?}", e);
            info!("account_register error {:?}", e.into_source().unwrap());

            status::Custom(err_status, String::from("sorry"))
                
        }
    }
  



//    status::Custom(Status::NotFound, String::from("sorry"))
}

#[derive(Deserialize, Debug)]
struct Token {
    token: String
}

use uuid::Uuid;

#[derive(Serialize, Debug)]
struct CharactersResponse {
    characters: Vec<Character>
}

#[post("/account/<username>/characters", format="json", data="<token>")]
async fn get_characters(mut db: Connection<PostgresDB>,
                  username: String,
                  token: Json<Token>) -> status::Custom<String> {
    info!("username: {:?} with token {:?} get_characters", username, token.token);

    let username_result = db.query(db::queries::GET_USER_TOKEN, &[&token.token]).await;

    match username_result {
        Ok(db_rows) => {

            if db_rows.len() < 1 {
                return status::Custom(Status::NotFound, String::from("sorry"));
            }

            let mut username: String = String::from("");
            
            for row in db_rows {
                username = row.get(0);
            }

            let characters_result = db.query(db::queries::GET_CHARACTERS_BY_ACCOUNT, &[&username]).await;

            match characters_result {
                Ok(char_rows) => {
                    info!("char rows {:?}", char_rows);

                    let mut characters: Vec<Character> = vec!();

                    for row in char_rows {
                        let id: String = row.get(0);
                        let name: String = row.get(1);
                        let level: i32 = row.get(2);
                        let experience: i32 = row.get(3);
                        let class: String = row.get(4);
                        let race: String = row.get(5);

                        let id_string = id.to_string();
                        let character = Character::new(id_string,
                                                       name,
                                                       level,
                                                       experience,
                                                       class,
                                                       race);

                        characters.push(character);
                    }

                    let json_string = json::to_string(
                        &CharactersResponse { characters }
                    ).unwrap();
                    return status::Custom(Status::new(213), json_string);
                                          
                },
                Err(e) => {
                    return status::Custom(Status::new(500), String::from("sorry"));
                }
            }
        },
            
        
        
        Err(e) => {
            error!("get_characters error {:?}", e);
            status::Custom(Status::new(500), String::from("sorry"))
        }
        
    }
}



struct DBConnParams {
    host: String,
    db_name: String,
    port: String,
    password: String,
    username: String
}


impl DBConnParams {
    pub fn get() -> Result<DBConnParams, AppError> {
        
        let host = env::var("DB_HOST")?;
        let password = env::var("DB_PASSWORD")?;
        let username = env::var("DB_USERNAME")?;
        let database = env::var("DB_DATABASE")?;
        let port = env::var("DB_PORT")?;

        Ok(DBConnParams {
            host: host, db_name: database, port: port, password: password, username: username
        })
    }

    pub fn to_string(&self) -> String {
        return format!("host={} port={} user={} password={} dbname={}", self.host, self.port, self.username, self.password, self.db_name);
    }
}

fn check_db() -> Result<String, AppError> {

    let connection_string = DBConnParams::get()?.to_string();

    info!("{}", connection_string);
    
    let mut client = Client::connect(&connection_string, NoTls)?;

    let data = None::<&[u8]>;

    let mut results: Vec<i32> = vec!();

    for row in client.query("SELECT 1", &[])? {
        let one: i32 = row.get(0);
        results.push(one);
    }

    if results.len() > 0 {
        Ok(results[0].to_string())
    } else {
        Err(AppError::new("Did not receive a result from the database".to_string()))
    }       
}

mod db;
use db::models;
use db::models::{Account,
                 Character,
                 Inventory,
                 Item,
};

use rocket::{Rocket, Build};

#[derive(Database)]
#[database("postgres_db")]
struct PostgresDB(rocket_db_pools::deadpool_postgres::Pool);

#[rocket::main]
async fn main() -> Result<(), AppError> {

    env_logger::init();

    let (tx, rx) = mpsc::channel();

    tokio::task::spawn_blocking(move || {

        let check_db_result = check_db();

        match check_db_result {
            Ok(v) => {
                println!("{:?}", v);
                tx.send((true, None)).unwrap()
            }
                
                
            ,
            Err(e) => tx.send((false, Some(e))).unwrap()        
        }
            
    });

    let pg_result = rx.recv().unwrap();

    if pg_result.0 {

        let connection_string = DBConnParams::get()?.to_string();

        let figment = rocket::Config::figment()
            .merge(("databases.postgres_db", rocket_db_pools::Config {
                url: connection_string, 
                min_connections: None,
                max_connections: 16,
                connect_timeout: 3,
                idle_timeout: None,
            }));

        let rocket = rocket::custom(figment)
            .attach(PostgresDB::init())
            .mount("/", routes![index,
                                login,
                                register,
                                get_characters
            ])
            .launch()
            .await;
        
        Ok(())
    } else {
        let error = pg_result.1.unwrap();
        error!("Received pg failure {:?}, not starting web app", error);
        Err(error)
    }
    
}
