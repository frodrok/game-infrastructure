
pub const GET_USER: &str = "SELECT username, password FROM account WHERE username = $1";

pub const INSERT_USER: &str = "INSERT INTO account (username, password) VALUES ($1, $2)";

pub const SET_USER_TOKEN: &str = "UPDATE account set access_token = $1 where username = $2";

pub const GET_USER_TOKEN: &str = "SELECT username FROM account where access_token = $1";

pub const GET_CHARACTERS_BY_ACCOUNT: &str = "SELECT id::text, name, level, experience, class, race from character where account_id = (select id from account where username = $1)";
