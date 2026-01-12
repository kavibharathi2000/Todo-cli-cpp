-- This create table for the db 
-- One basic task is also add for testing 
CREATE TABLE tasks(
    id INT PRIMARY KEY,
    task  VARCHAR(50),
    done_status BOOLEAN
)

INSERT INTO tasks VALUES(1,"lunch",true);