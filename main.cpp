#include<iostream>
#include<sqlite3.h>
#include<vector>


using namespace std;


constexpr const char* db_path = "database.db";

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
            else if (colName == "task_status" && argv[i]) t.task_status = std::stoi(argv[i]);
         } 
        tasks->push_back(t); return 0;
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

    bool InsertTaskDB(){
        // Function to insert the task into the db
        
    }



    public:
    sqlite3* db;
    char *errorMsg =0;


    ToDo(){
        db_connect();
    }


    // List Task 
    void ListTask(){
        std::vector<task_data>tasks;
        tasks = GetAlltasks();
        cout<<"This is the List task function"<<endl;
        for(const auto& task : tasks){
            cout<<"ID:-"<<task.id<<"\tTask:-"<<task.task<<"\tStatus:-"<<(task.task_status?"done":"not done")<<endl;
        }
    }


    // Update Task 
    // Add task 
    // Delete Task 
};








int main(){
    ToDo tasker;
    tasker.ListTask();
    cout<<"The execution has been Terminated"<<endl;
}

