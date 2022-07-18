use std::env::VarError::*;
use std::env::VarError;
use postgres::Error;
use std::error::Error as OtherError;


// TODO: Option<String> + error: Error
#[derive(Debug)]
pub struct AppError {
    message: String
}

impl AppError {
    pub fn new(message: String) -> Self {
        AppError { message }
    }
}

impl From<std::string::String> for AppError {
    fn from(str: String) -> Self {
        AppError { message: str }
    }
}

impl From<postgres::Error> for AppError {
    fn from(error: postgres::Error) -> Self {
        AppError { message: error.to_string() }
    }
}

impl From<VarError> for AppError {
    fn from(error: VarError) -> Self {
        match error {
            NotPresent => AppError { message: "Env var not present".to_string() },
            NotUnicode(_) => AppError { message: "Env var not unicode".to_string() }
        }
    }
}
