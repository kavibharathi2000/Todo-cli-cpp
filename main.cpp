#include<iostream>
#include<sqlite3.h>
#include<vector>


using namespace std;


constexpr const char* db_path = "database.db";
constexpr const char* table ="tasks";

struct task_data{
    int id;
    string task;
    bool task_status;
};

class ToDo{
    void db_connect(){
        //  Function to Connect the Database
        int rc = sqlite3_open(db_path, &db);
        if(rc){
            cerr<<"[-] Unable to connect to DB: "<<sqlite3_errmsg(db)<<endl;
        }
        else{
            cout<<"[*] Connected to DB..."<<endl;
        } 
    }

    static int collectCallback(void* data, int argc, char** argv, char** azColName) { 
        auto* tasks = static_cast<std::vector<task_data>*>(data); 
        task_data t;
        for (int i = 0; i < argc; i++) { 
            std::string colName = azColName[i]; 
            if (colName == "id" && argv[i]) t.id = std::stoi(argv[i]); 
            else if (colName == "task" && argv[i]) t.task = argv[i]; 
            else if (colName == "done_status" && argv[i]) t.task_status = std::stoi(argv[i]);
         } 
        tasks->push_back(t); 
        return 0;
        }

    std::vector<task_data> GetAlltasks(){
        // Function to get all task in DB
        const char *sql_list_all = "SELECT * FROM tasks";
        std::vector<task_data>output;
        int rc = sqlite3_exec(db,sql_list_all, collectCallback, &output ,&errorMsg);
        if(rc != SQLITE_OK){
            cerr<<"[-] "<<errorMsg<<endl;
            sqlite3_free(errorMsg);
        }
        else{
            cout<<"[-]Task Listed "<<endl;
        }
        return output;
    }

    bool InsertTaskDB(task_data data) {
        // Function to insert data to DB
        const char* sql = "INSERT INTO tasks (id, task, done_status) VALUES (?, ?, ?);"; 
        sqlite3_stmt* stmt;
        
        int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr);
        if (rc != SQLITE_OK) { 
            cerr << "[-] Failed to prepare statement: " << sqlite3_errmsg(db) << endl; 
            return false; 
        }

        // Bind the values
        sqlite3_bind_int(stmt, 1, data.id);
        sqlite3_bind_text(stmt, 2, data.task.c_str(), -1, SQLITE_TRANSIENT); 
        sqlite3_bind_int(stmt, 3, data.task_status ? 1 : 0);

        // Execute the statement
        rc = sqlite3_step(stmt);   
        if (rc != SQLITE_DONE) { 
            cerr << "[-] Insert failed: " << sqlite3_errmsg(db) << endl; 
            sqlite3_finalize(stmt); 
            return false; 
        } 

        sqlite3_finalize(stmt); 
        cout << "[*] Task inserted successfully!" << endl; 
        return true;
    }

    bool UpdateTaskDB(int id, bool new_status) {
        // Function to Update the task in DB
        const char* sql = "UPDATE tasks SET done_status = ? WHERE id = ?;";
        sqlite3_stmt* stmt;

        int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr);
        if (rc != SQLITE_OK) {
            cerr << "[-] Failed to prepare update: " << sqlite3_errmsg(db) << endl;
            return false;
        }

        // Bind values
        sqlite3_bind_int(stmt, 1, new_status ? 1 : 0);
        sqlite3_bind_int(stmt, 2, id);

        rc = sqlite3_step(stmt);
        if (rc != SQLITE_DONE) {
            cerr << "[-] Update failed: " << sqlite3_errmsg(db) << endl;
            sqlite3_finalize(stmt);
            return false;
        }

        sqlite3_finalize(stmt);
        cout << "[*] Task updated successfully!" << endl;
        return true;
    }

    bool DeleteTaskDB(int id) {
        // Function to delete the task from DB
        const char* sql = "DELETE FROM tasks WHERE id = ?;";
        sqlite3_stmt* stmt;

        int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr);
        if (rc != SQLITE_OK) {
            cerr << "[-] Failed to prepare delete: " << sqlite3_errmsg(db) << endl;
            return false;
        }

        // Bind ID
        sqlite3_bind_int(stmt, 1, id);

        rc = sqlite3_step(stmt);
        if (rc != SQLITE_DONE) {
            cerr << "[-] Delete failed: " << sqlite3_errmsg(db) << endl;
            sqlite3_finalize(stmt);
            return false;
        }

        sqlite3_finalize(stmt);
        cout << "[*] Task deleted successfully!" << endl;
        return true;
    }





    public:
    sqlite3* db;
    char *errorMsg =0;
    
    ToDo(){
        db_connect();
    }

    void ListTask(){
        // Function to list the task 
        std::vector<task_data>tasks;
        tasks = GetAlltasks();
        for(const auto& task : tasks){
            cout<<"ID:-"<<task.id<<"\tTask:-"<<task.task<<"\tStatus:-"<<(task.task_status?"done":"not done")<<endl;
        }
    }

    void AddTask(string task,bool status){
        // Function to Add the task
        task_data new_task_data;
        new_task_data.id = 3;
        new_task_data.task = task;
        new_task_data.task_status = status;
        bool output = InsertTaskDB(new_task_data);
        if(output){
            cout<<"[-]Task has been Inserted..."<<endl;
        }
        else{
            cout<<"[-] Unable to Insert  the task..."<<endl;
        }
    }

    void UpdateTask(int id, bool status){
        // Function to update task 
        bool output = UpdateTaskDB(id,status);
        if(output){
            cout<<"[-]Task has been Updated..."<<endl;
        }
        else{
            cout<<"[-] Unable to update the task..."<<endl;
        }
    }

    void DeleteTask(int id){
        // Function to delete the task 
        bool output = DeleteTaskDB(id);
        if(output){
            cout<<"[-]Task has been Deleted..."<<endl;
        }
        else{
            cout<<"[-] Unable to Delete the task..."<<endl;
        }

    }


};








int main(){
    cout<<"Execution has been started "<<endl;
    cout<<endl;
    ToDo tasker;
    cout<<endl;
    cout<<"Listing the task that we have "<<endl;
    tasker.ListTask();
    cout<<endl;
    
    // string task = "reading";
    // bool sts = false;
    // cout<<endl;
    // cout<<"Adding the Task"<<endl;
    // bool task_ins = tasker.AddTask(task,sts);
    
    // if(task_ins){
    //     cout<<"The Task  has been added"<<endl;
    // }else{
    //     cout<<"[-] Unable to Add the Task "<<endl;
    // }
    // cout<<endl;

    // cout<<"Listing the task after adding the task to the database"<<endl;
    // cout<<endl;
    // tasker.ListTask();
    // cout<<endl;
    cout<<"The execution has been Terminated"<<endl;
}

