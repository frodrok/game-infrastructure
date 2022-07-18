use uuid::Uuid;

use std::collections::HashMap;
use rocket::serde::Deserialize;
use rocket::serde::Serialize;
use rocket::serde::json::Json;


#[derive(Debug, Serialize, Deserialize)]
pub struct Account {
    id: String,
    username: String,
    password: String,
    characters: Vec<Character>
}

#[derive(Debug, Serialize, Deserialize)]
enum Class {
    Rogue,
    Mage,
    Priest,
    Shaman,
    Warlock,
    Warrior,
    Druid,
    Paladin
}

#[derive(Debug, Serialize, Deserialize)]
enum Race {
    Human,
    Dwarf,
    Draenei,
    Gnome,
    NightElf,
    Orc,
    Tauren,
    Undead,
    BloodElf,
    Troll
}

#[derive(Serialize, Deserialize, Debug)]
pub struct Character {
    id: String,
    name: String,
    level: usize,
    experience: u64,
    class: Class,
    race: Race,
    inventory: Option<Inventory>,
    bank: Option<Bank>
}

impl Character {
    pub fn new(id: String, name: String,
               level: i32, experience: i32,
               class: String, race: String) -> Self {

        let class_enum = match class {
            _ => Class::Mage
        };
        let race_enum = match race {
            _ => Race::Undead
        };
        
        Character {
            id: id,
            name: name,
            level: level.try_into().unwrap(),
            experience: experience.try_into().unwrap(),
            class: class_enum,
            race: race_enum,
            inventory: None,
            bank: None,
        }
    }
}

#[derive(Debug, Serialize, Deserialize)]
pub struct Inventory {
    id: String,
    items: Vec<Item>
}

#[derive(Debug, Serialize, Deserialize)]
pub struct Bank {
    id: String,
    items: Vec<Item>
}

#[derive(Debug, Serialize, Deserialize)]
pub struct Item {
    id: String,
    name: String,
}

