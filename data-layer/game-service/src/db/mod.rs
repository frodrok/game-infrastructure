pub mod models;

pub mod queries;


/* Fields is assumed to be Vec<(field_name, field_type)> */
pub fn to_create_table_query(table_name: String, fields: Vec<(String, String)>) -> String {
    let mut x = format!("CREATE TABLE {} (", table_name);

    for field in fields {
        x.push_str(&field.0);
        x.push(',');
        x.push_str(&field.1);
        x.push(',');
    }

    x.push(')');

    x
}
