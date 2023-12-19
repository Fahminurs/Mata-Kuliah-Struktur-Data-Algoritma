#include <iostream>
#include <mysql.h>
#include <chrono>
#include <sstream>
#include <vector>
#include <iomanip>
#include <numeric>
#include <algorithm>
#include <map>
#include <conio.h>
#include <thread>

using namespace std;
using namespace chrono;
int qstate=0;
std::map<std::string, std::string> cariMap = {
    {"1", "Judul"},
    {"2", "Pengarang"},
    {"3", "Penerbit"},
    {"4", "Tahun_Terbit"},
    {"5", "ISBN"},
    {"8", "Rak"},
    {"9", "Genre"},
    {"10", "Status"}
};
////////// Sorting //////////////////////
double waktu_bubblesort, waktu_Insertion, waktu_Selection;
int iterasi_bubblesort, iterasi_Insertion, iterasi_Selection;

////////// Search //////////////////////
double Waktu_squensial, Waktu_Binary,Binary_sort;
int iterasi_squensial, iterasi_Binary;


int Cek_Same_User(MYSQL *conn, string usm, string pwd) {
    // Check if the username and password already exist
    stringstream checkQuery;
    checkQuery << "SELECT COUNT(*) FROM tbl_user1 WHERE username = '" << usm << "' AND password = '" << pwd << "';";
    string checkQueryString = checkQuery.str();
    const char* checkQueryChar = checkQueryString.c_str();

    int checkQstate = mysql_query(conn, checkQueryChar);
    if (checkQstate != 0) {
        cerr << "Error: " << mysql_error(conn) << endl;
        return -1; // or any suitable error code
    }

    MYSQL_RES* checkResult = mysql_store_result(conn);
    if (checkResult == nullptr) {
        cerr << "Error: " << mysql_error(conn) << endl;
        return -1; // or any suitable error code
    }

    MYSQL_ROW checkRow = mysql_fetch_row(checkResult);
    int count = atoi(checkRow[0]);
    mysql_free_result(checkResult);

    return count;
}


void From_registrasi(MYSQL* conn, string pil_reg) {
    string usm, pwd, nama, email;
    stringstream ss;//deklarasi ss
    int role=1;
    int id = 0;
    int notlp;


    cout << "Masukkan nama : ";
    getline(cin, nama);
    cout << "Masukkan email : "; cin >> email;
    cout << "Masukkan Username : "; cin >> usm;
    cout << "Masukkan Password : "; cin >> pwd;
    cout << "Masukkan No.Telpon : "; cin >> notlp;




     if (Cek_Same_User(conn, usm, pwd) == 0){

    // Mendapatkan waktu saat ini dari sistem
    auto now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    // Mengonversi waktu ke dalam format string
    char buffer[80];
    strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", localtime(&now));

    // Menambahkan user baru jika username dan password belum terdaftar
    ss << "INSERT INTO tbl_user1 (id_user, nama, email, username, password, no_tlp, role, date_created) VALUES (" <<
        id << ",'" << nama << "','" << email << "','" << usm << "','" << pwd << "'," << notlp << "," << role << ",'" << buffer << "')";

    string query = ss.str();
    const char* q = query.c_str();
    qstate = mysql_query(conn, q);

    if (qstate == 0) {
            cout << qstate;
        if(role == 0){
            cout << "Admin Baru Berhasil Ditambahkan" << endl;
            //return 0;
        } else {
            cout << "User Baru Berhasil Ditambahkan" << endl;
          //  return 0;
        }
}else{

cout << "Quey Gagal";
}
}else{
cout << "Username dan password sudah terdaftar. Silakan gunakan yang lain." << endl;
}
}

int Login(MYSQL* conn) {
    string usm, pwd;
cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
cin.ignore();
    cout << "Masukkan Username : ";
    getline(cin,usm);
    cout << "Masukkan Password : "; cin >> pwd;

    if (Cek_Same_User(conn, usm, pwd) == 1) {
        cout << "Login berhasil!" << endl;
        // Lakukan operasi sesuai dengan kebutuhan setelah login berhasil
    } else {
        cout << "Login gagal. Username atau password salah." << endl;
    }
    return 1;
}

void tampil_data(MYSQL* conn) {
    const char* query = "SELECT * FROM `data_buku1` WHERE 1";
    int qstate = mysql_query(conn, query);

    if (qstate == 0) {
        MYSQL_RES* result = mysql_store_result(conn);

        if (result != nullptr) {
            MYSQL_ROW row;
            MYSQL_FIELD* field;
            int num_fields = mysql_num_fields(result);

            vector<int> column_widths(num_fields, 0);

            // Mengambil lebar maksimum untuk setiap kolom
            while ((row = mysql_fetch_row(result))) {
                for (int i = 0; i < num_fields; i++) {
                    if (row[i] != nullptr) {
                        int length = strlen(row[i]);
                        if (length > column_widths[i]) {
                            column_widths[i] = length;
                        }
                    }
                }
            }

            // Menampilkan header kolom
            for (int i = 0; i < num_fields; i++) {
                field = mysql_fetch_field(result);
                cout << setw(column_widths[i] + 2) << left << field->name;
            }
            cout << endl;

            // Menampilkan garis pembatas
            for (int i = 0; i < accumulate(column_widths.begin(), column_widths.end(), 0) + 2 * (num_fields - 1); i++) {
                cout << "-";
            }
            cout << endl;

            // Menampilkan data
            mysql_data_seek(result, 0); // Mengatur kursor baris ke awal
            while ((row = mysql_fetch_row(result))) {
                for (int i = 0; i < num_fields; i++) {
                    if (row[i] != nullptr) {
                        cout << setw(column_widths[i] + 2) << left << row[i];
                    } else {
                        cout << setw(column_widths[i] + 2) << left << "NULL";
                    }
                }
                cout << endl;
            }

            mysql_free_result(result);
        } else {
            cerr << "Error: " << mysql_error(conn) << endl;
        }
    } else {
        cerr << "Query Error : " << mysql_error(conn) << endl;
    }
}


// Fungsi untuk memotong teks jika lebih dari 2 kata
string truncateText(const string& text, int maxLength) {
    string truncatedText = text;
    size_t spacePos = truncatedText.find(' ');

    if (spacePos != string::npos) {
        spacePos = truncatedText.find(' ', spacePos + 2); // Cari kata kedua
        if (spacePos != string::npos) {
            truncatedText = truncatedText.substr(0, spacePos) + "...";
        }
    }

    return truncatedText.substr(0, maxLength);
}

void display_sorting(const vector<vector<string>>& sortedData) {
    int n = sortedData.size();
const int NUM_COLUMNS = 10;
    // Hitung lebar maksimum untuk setiap kolom
    vector<int> columnWidths(NUM_COLUMNS, 2); // 10 adalah jumlah kolom

    for (int i = 0; i < n; i++) {
        for (int j = 0; j < NUM_COLUMNS; ++j) {
            columnWidths[j] = max(columnWidths[j], static_cast<int>(sortedData[i][j].length()));
        }
    }

    // Display header
    cout << setw(columnWidths[0] + 1) << left << "Judul";
    cout << setw(columnWidths[1] + 1) << left << "  Penulis";
    cout << setw(columnWidths[2] + 1) << left << "   Penerbit";
    cout << setw(columnWidths[3] + 1) << left << "Tahun";
    cout << setw(columnWidths[4] + 1) << left << "        ISBN";
    cout << setw(columnWidths[5] + 1) << left << "   Halaman";
    cout << setw(columnWidths[6] + 1) << left << "Stok";
    cout << setw(columnWidths[7] + 1) << left << "Rak";
    cout << setw(columnWidths[8] + 1) << left << "  Genre";
    cout << setw(columnWidths[9] + 1) << left << "  Status" << endl;

    // Display separator
for (int i = 0; i < NUM_COLUMNS; ++i) {
    cout << setw(columnWidths[i] + 2) << left << setfill('-') << "" << setfill(' ');
}
cout << endl;

    // Display data
    for (int i = 0; i < n-1; i++) {
        for (int j = 0; j < NUM_COLUMNS; ++j) {
            // Memotong teks jika lebih dari 2 kata dan menyesuaikan lebar
            cout << setw(columnWidths[j] + 2) << left << truncateText(sortedData[i][j], columnWidths[j]);
        }
        cout << endl;
    }
}


void chronoSortingFunction(void (*sortingFunction)(vector<vector<string>>&), vector<vector<string>>& data, const string& sortingMethodName) {
     vector<vector<string>> dataCopy = data;
    auto start = high_resolution_clock::now();

    sortingFunction(data);

    auto stop = high_resolution_clock::now();
    auto duration = duration_cast<milliseconds>(stop - start);

    cout << "Sorting time using " << sortingMethodName << ": " << duration.count() << " milliseconds" << endl;
}


void Bubblesort(MYSQL* conn,int kapasitas) {
    string limitkapasitas = "LIMIT " + to_string(kapasitas);
    string query = "SELECT * FROM `data_buku1` " + limitkapasitas;
    const char* q = query.c_str();
    int qstate = mysql_query(conn, q);

    if (qstate == 0) {
        MYSQL_RES* result = mysql_store_result(conn);

        if (result != nullptr) {
            MYSQL_ROW row;
            vector<vector<string>> rowData;

            // Fetch and store data from the database
            while ((row = mysql_fetch_row(result))) {
                vector<string> currentRow;

                for (int i = 1; i <= 10; ++i) {
                    if (row[i] != nullptr) {
                        currentRow.push_back(row[i]);
                    } else {
                        currentRow.push_back("");
                    }
                }

                rowData.push_back(currentRow);
            }

            // Mengukur waktu eksekusi menggunakan chrono
            auto start = chrono::high_resolution_clock::now();

            // Bubble Sort for the first column (Judul)
            int n = rowData.size();
            bool tukar;
            int iterasi = 0;  // Inisialisasi variabel iterasi

            do {
                tukar = false;

                for (int i = 0; i < n - 1; i++) {
                    // Modify the comparison logic here for the first column (Judul)
                    if (rowData[i][0].compare(rowData[i + 1][0]) > 0) {
                        swap(rowData[i], rowData[i + 1]);
                        tukar = true;
                    }
                   // cout << "\nIterasi = " << iterasi;
                    iterasi++;  // Increment iterasi setiap kali melakukan pertukaran
                }

            } while (tukar);

            auto end = chrono::high_resolution_clock::now();
            chrono::duration<double> duration = end - start;

            // Display the sorted data for the first column (Judul)
      cout << "\n\e[1;34mData setelah diurutkan berdasarkan Judul (Bubble Sort):\e[0m\n";
//            display_sorting(rowData);

            cout << "Waktu eksekusi: " << duration.count() << " Detik" << endl;
            cout << "Jumlah iterasi: " << iterasi << endl;  // Menampilkan jumlah iterasi
                waktu_bubblesort = duration.count();
                iterasi_bubblesort = iterasi;


            mysql_free_result(result);
        } else {
            cerr << "Error: " << mysql_error(conn) << endl;
        }
    } else {
        cerr << "Query Error: " << mysql_error(conn) << endl;
    }
}

void Bubblesort_Dsc(MYSQL* conn,int kapasitas) {
    string limitkapasitas = "LIMIT " + to_string(kapasitas);
    string query = "SELECT * FROM `data_buku1` " + limitkapasitas;
    const char* q = query.c_str();
    int qstate = mysql_query(conn, q);

    if (qstate == 0) {
        MYSQL_RES* result = mysql_store_result(conn);

        if (result != nullptr) {
            MYSQL_ROW row;
            vector<vector<string>> rowData;

            // Fetch and store data from the database
            while ((row = mysql_fetch_row(result))) {
                vector<string> currentRow;

                for (int i = 1; i <= 10; ++i) {
                    if (row[i] != nullptr) {
                        currentRow.push_back(row[i]);
                    } else {
                        currentRow.push_back("");
                    }
                }

                rowData.push_back(currentRow);
            }

            // Mengukur waktu eksekusi menggunakan chrono
            auto start = chrono::high_resolution_clock::now();

            // Bubble Sort for the first column (Judul)
            int n = rowData.size();
            bool tukar;
            int iterasi = 0;  // Inisialisasi variabel iterasi

            do {
                tukar = false;

                for (int i = 0; i < n - 1; i++) {
                    // Modify the comparison logic here for the first column (Judul)
                    if (rowData[i][0].compare(rowData[i + 1][0]) < 0) {
                        swap(rowData[i], rowData[i + 1]);
                        tukar = true;
                    }
                   // cout << "\nIterasi = " << iterasi;
                    iterasi++;  // Increment iterasi setiap kali melakukan pertukaran
                }

            } while (tukar);

            auto end = chrono::high_resolution_clock::now();
            chrono::duration<double> duration = end - start;

            // Display the sorted data for the first column (Judul)
      cout << "\n\e[1;34mData setelah diurutkan berdasarkan Judul (Bubble Sort):\e[0m\n";
//            display_sorting(rowData);

            cout << "Waktu eksekusi: " << duration.count() << " Detik" << endl;
            cout << "Jumlah iterasi: " << iterasi << endl;  // Menampilkan jumlah iterasi
                waktu_bubblesort = duration.count();
                iterasi_bubblesort = iterasi;


            mysql_free_result(result);
        } else {
            cerr << "Error: " << mysql_error(conn) << endl;
        }
    } else {
        cerr << "Query Error: " << mysql_error(conn) << endl;
    }
}

void Selection_sort(MYSQL* conn,int kapasitas) {
string limitkapasitas = "LIMIT " + to_string(kapasitas);
    string query = "SELECT * FROM `data_buku1` " + limitkapasitas;
    const char* q = query.c_str();
    int qstate = mysql_query(conn, q);

    if (qstate == 0) {
        MYSQL_RES* result = mysql_store_result(conn);

        if (result != nullptr) {
            MYSQL_ROW row;
            vector<vector<string>> data;

            // Fetch and store data from the database
            while ((row = mysql_fetch_row(result))) {
                vector<string> rowData;
                for (int i = 1; i < mysql_num_fields(result); ++i) {
                    if (row[i] != nullptr) {
                        rowData.push_back(row[i]);
                    } else {
                        rowData.push_back(""); // Handle NULL values
                    }
                }
                data.push_back(rowData);
            }
                auto start = chrono::high_resolution_clock::now(); //chrono
            // Sorting using Selection Sort for the first column (Judul)
            int n = data.size();
            int iterasi = 0;
            for (int i = 0; i < n - 1; i++) {
                int minIndex = i;
                for (int j = i + 1; j < n; j++) {
                    // Modify the comparison logic based on data type (assuming strings for now)
                    if (data[j][0] < data[minIndex][0]) {  // Compare with the first column (Judul)
                        minIndex = j;
                    }
                    iterasi++;
                }

                // Swap the found minimum element with the first element
                if (minIndex != i) {
                    swap(data[i], data[minIndex]);
                }
            }
                    auto end = chrono::high_resolution_clock::now();
            chrono::duration<double> duration = end - start;

            // Display the sorted data for the first column (Judul)
           cout << "\n\e[1;34mData setelah diurutkan berdasarkan Judul (Selection Sort):\e[0m\n";
    //            display_sorting(data);
 cout << "Waktu eksekusi: " << duration.count() << " Detik" << endl;

 cout << "Jumlah iterasi: " << iterasi << endl;
waktu_Selection = duration.count();

iterasi_Selection = iterasi;
            mysql_free_result(result);
        } else {
            cerr << "Error: " << mysql_error(conn) << endl;
        }
    } else {
        cerr << "Query Error: " << mysql_error(conn) << endl;
    }
}

void Selection_sort_Dsc(MYSQL* conn,int kapasitas) {
string limitkapasitas = "LIMIT " + to_string(kapasitas);
    string query = "SELECT * FROM `data_buku1` " + limitkapasitas;
    const char* q = query.c_str();
    int qstate = mysql_query(conn, q);

    if (qstate == 0) {
        MYSQL_RES* result = mysql_store_result(conn);

        if (result != nullptr) {
            MYSQL_ROW row;
            vector<vector<string>> data;

            // Fetch and store data from the database
            while ((row = mysql_fetch_row(result))) {
                vector<string> rowData;
                for (int i = 1; i < mysql_num_fields(result); ++i) {
                    if (row[i] != nullptr) {
                        rowData.push_back(row[i]);
                    } else {
                        rowData.push_back(""); // Handle NULL values
                    }
                }
                data.push_back(rowData);
            }
                auto start = chrono::high_resolution_clock::now(); //chrono
            // Sorting using Selection Sort for the first column (Judul)
            int n = data.size();
            int iterasi = 0;
            for (int i = 0; i < n - 1; i++) {
                int minIndex = i;
                for (int j = i + 1; j < n; j++) {
                    // Modify the comparison logic based on data type (assuming strings for now)
                    if (data[j][0] > data[minIndex][0]) {
                        minIndex = j;
                    }
                    iterasi++;
                }

                // Swap the found minimum element with the first element
                if (minIndex != i) {
                    swap(data[i], data[minIndex]);
                }
            }
                    auto end = chrono::high_resolution_clock::now();
            chrono::duration<double> duration = end - start;

            // Display the sorted data for the first column (Judul)
           cout << "\n\e[1;34mData setelah diurutkan berdasarkan Judul (Selection Sort):\e[0m\n";
           cout << "\nDescending :\n";
//            display_sorting(data);
 cout << "Waktu eksekusi: " << duration.count() << " Detik" << endl;

 cout << "Jumlah iterasi: " << iterasi << endl;
waktu_Selection = duration.count();

iterasi_Selection = iterasi;
            mysql_free_result(result);
        } else {
            cerr << "Error: " << mysql_error(conn) << endl;
        }
    } else {
        cerr << "Query Error: " << mysql_error(conn) << endl;
    }
}

void insertion_sort(MYSQL* conn, int kapasitas) {
string limitkapasitas = "LIMIT " + to_string(kapasitas);
    string query = "SELECT * FROM `data_buku1` " + limitkapasitas;
    const char* q = query.c_str();
    int qstate = mysql_query(conn, q);

    if (qstate == 0) {
        MYSQL_RES* result = mysql_store_result(conn);

        if (result != nullptr) {
            MYSQL_ROW row;
            vector<vector<string>> data;

            // Fetch and store data from the database
            while ((row = mysql_fetch_row(result))) {
                vector<string> rowData;
                for (int i = 1; i <= 10; ++i) {
                    if (row[i] != nullptr) {
                        rowData.push_back(row[i]);
                    } else {
                        rowData.push_back(""); // Handle NULL values
                    }
                }
                data.push_back(rowData);
            }

            auto start = chrono::high_resolution_clock::now();
            // Sorting using Insertion Sort for the first column (Judul)
            int n = data.size();
            int iterasi = 0;
            for (int i = 1; i < n; i++) {
                vector<string> data_sisip = data[i];
                int j = i - 1;

                while (j >= 0 && data[j][0] > data_sisip[0]) {
                    data[j + 1] = data[j];
                    j = j - 1;
                    iterasi++;
                }

                data[j + 1] = data_sisip;
            }
            auto end = chrono::high_resolution_clock::now();
            chrono::duration<double> duration = end - start;

            // Display the sorted data for the first column (Judul)
 cout << "\n\e[1;34mData setelah diurutkan berdasarkan Judul (Insertion Sort):\e[0m\n";

//display_sorting(data);
cout << "Waktu eksekusi: " << duration.count() << " Detik" << endl;
cout << "Jumlah iterasi: " << iterasi << endl;
waktu_Insertion = duration.count();
    iterasi_Insertion = iterasi;

            mysql_free_result(result);
        } else {
            cerr << "Error: " << mysql_error(conn) << endl;
        }
    } else {
        cerr << "Query Error: " << mysql_error(conn) << endl;
    }

}

void insertion_sort_dsc(MYSQL* conn, int kapasitas) {
string limitkapasitas = "LIMIT " + to_string(kapasitas);
    string query = "SELECT * FROM `data_buku1` " + limitkapasitas;
    const char* q = query.c_str();
    int qstate = mysql_query(conn, q);

    if (qstate == 0) {
        MYSQL_RES* result = mysql_store_result(conn);

        if (result != nullptr) {
            MYSQL_ROW row;
            vector<vector<string>> data;

            // Fetch and store data from the database
            while ((row = mysql_fetch_row(result))) {
                vector<string> rowData;
                for (int i = 1; i <= 10; ++i) {
                    if (row[i] != nullptr) {
                        rowData.push_back(row[i]);
                    } else {
                        rowData.push_back(""); // Handle NULL values
                    }
                }
                data.push_back(rowData);
            }

            auto start = chrono::high_resolution_clock::now();
            // Sorting using Insertion Sort for the first column (Judul)
            int n = data.size();
            int iterasi = 0;
            for (int i = 1; i < n; i++) {
                vector<string> data_sisip = data[i];
                int j = i - 1;

                while (j >= 0 && data[j][0] < data_sisip[0]) {
                    data[j + 1] = data[j];
                    j = j - 1;
                    iterasi++;
                }

                data[j + 1] = data_sisip;
            }
            auto end = chrono::high_resolution_clock::now();
            chrono::duration<double> duration = end - start;

            // Display the sorted data for the first column (Judul)
 cout << "\n\e[1;34mData setelah diurutkan berdasarkan Judul (Insertion Sort):\e[0m\n";

//display_sorting(data);
cout << "Waktu eksekusi: " << duration.count() << " Detik" << endl;
cout << "Jumlah iterasi: " << iterasi << endl;
waktu_Insertion = duration.count();
    iterasi_Insertion = iterasi;

            mysql_free_result(result);
        } else {
            cerr << "Error: " << mysql_error(conn) << endl;
        }
    } else {
        cerr << "Query Error: " << mysql_error(conn) << endl;
    }

}



void hasil_Sorting() {

cout << endl<< endl;
    // Header
    cout << "+-------------------------------------------------------------+\n";
    cout << "|                Perbandingan Algoritma Sorting               |\n";
    cout << "+-------------------+--------------------+--------------------+\n";
    cout << "|\e[1;31m    Bubble sort\e[0m    |\e[1;32m   Insertion sort   |\e[1;36m   Selection sort   \e[0m|\n";
    cout << "+---------+---------+----------+---------+----------+---------+\n";
    cout << "|  Waktu  | Iterasi |   Waktu  | Iterasi |   Waktu  | Iterasi |\n";
    cout << "+---------+---------+----------+---------+----------+---------+\n";

    // Data rows
    cout << "|" << setw(1) << fixed << setprecision(2) << waktu_bubblesort << "detik"
         << "|" << setw(9) << iterasi_bubblesort
         << "|" << setw(3) << fixed << setprecision(2) << waktu_Insertion << "detik"
         << "|" << setw(9) << iterasi_Insertion
         << "|" << setw(4) << fixed << setprecision(2) << waktu_Selection << "detik"
         << "|" << setw(9) << iterasi_Selection << "|\n";

    // Footer
    cout << "+---------+---------+----------+---------+----------+---------+\n";
    cout << "|                Algoritma Efisien berdasarkan                |\n";
    cout << "+-------------------+--------------------+--------------------+\n";
    cout << "|       waktu       |       iterasi      |  Waktu dan Iterasi |\n";
    cout << "+-------------------+--------------------+--------------------+\n";

    // Finding the most efficient algorithm based on time
    string Algoritma_efisien_waktu;
    if (waktu_bubblesort <= waktu_Insertion && waktu_bubblesort <= waktu_Selection) {
        Algoritma_efisien_waktu = "\e[1;31mBubble Sort\e[0m";
    } else if (waktu_Insertion <= waktu_bubblesort && waktu_Insertion <= waktu_Selection) {
        Algoritma_efisien_waktu = "\e[1;32mInsertion Sort\e[0m";
    } else {
        Algoritma_efisien_waktu = "\e[1;36mSelection Sort\e[0m";
    }

    // Displaying the most efficient algorithm based on time
    cout << "|" << setw(19) << Algoritma_efisien_waktu << "|";

    // Finding the most efficient algorithm based on iterations
    string Algoritma_efisien_iterasi;
    if (iterasi_bubblesort <= iterasi_Insertion && iterasi_bubblesort <= iterasi_Selection) {
        Algoritma_efisien_iterasi = "\e[1;31mBubble Sort\e[0m";
    } else if (iterasi_Insertion <= iterasi_bubblesort && iterasi_Insertion <= iterasi_Selection) {
        Algoritma_efisien_iterasi = "\e[1;32mInsertion Sort\e[0m";
    } else {
        Algoritma_efisien_iterasi = "\e[1;36mSelection Sort\e[0m";
    }

    // Displaying the most efficient algorithm based on iterations
    cout << setw(20) << Algoritma_efisien_iterasi << " |";

    // Finding the most efficient algorithm based on both time and iterations
    string Algoritma_efisien_waktu_iterasi;
    if ((waktu_bubblesort + iterasi_bubblesort) <= (waktu_Insertion + iterasi_Insertion) &&
        (waktu_bubblesort + iterasi_bubblesort) <= (waktu_Selection + iterasi_Selection)) {
        Algoritma_efisien_waktu_iterasi = "\e[1;31mBubble Sort\e[0m";
    } else if ((waktu_Insertion + iterasi_Insertion) <= (waktu_bubblesort + iterasi_bubblesort) &&
               (waktu_Insertion + iterasi_Insertion) <= (waktu_Selection + iterasi_Selection)) {
        Algoritma_efisien_waktu_iterasi = "\e[1;32mInsertion Sort\e[0m";
    } else {
        Algoritma_efisien_waktu_iterasi = "\e[1;36mSelection Sort\e[0m";
    }

    // Displaying the most efficient algorithm based on both time and iterations
    cout << setw(20) << Algoritma_efisien_waktu_iterasi << " |\n";

    // Closing
    cout << "+-------------------+--------------------+--------------------+\n";
}



std::string toLower(const std::string& str) {
    std::string result = str;
    transform(result.begin(), result.end(), result.begin(),
                   [](unsigned char c) { return std::tolower(c); });
    return result;
}


int Sequential_search(MYSQL * conn, string cari, string pilih) {
int kapasitas= 10000;
string limitkapasitas = "LIMIT " + to_string(kapasitas);

    string query = "SELECT * FROM `data_buku1` " + limitkapasitas;
    const char* q = query.c_str();
    int qstate = mysql_query(conn, q);
    MYSQL_RES * result = mysql_store_result(conn);
    std::string pilihLower;
    if (result != nullptr) {
        MYSQL_ROW row;
        int i = 0;
        bool datanya_ketemu = false;

        if (cari == "1" || cari == "5") {
            pilihLower = toLower(pilih);
            int convert = stoi(cari);
        cout << "\nHasil Squensial Search : \n";
        auto start = chrono::high_resolution_clock::now();
            while ((row = mysql_fetch_row(result)) && !datanya_ketemu) {
                //cout << "Iterasi : " << i + 1 << endl;

                // Convert database title to lowercase
                std::string titleLower = (row[convert] != nullptr) ? toLower(row[convert]) : "";

                if (titleLower == pilihLower) {
                    datanya_ketemu = true;
                    cout << "\nData ditemukan pada iterasi ke-" << i + 1 << ":\n";
                    iterasi_squensial = i + 1;
                    // Display the data or perform any other actions
                    auto end = chrono::high_resolution_clock::now();
                    chrono::duration<double, milli> duration = end - start;
                    Waktu_squensial =duration.count();

                    for (int j = 0; j < mysql_num_fields(result); ++j) {
                        cout << mysql_fetch_field_direct(result, j) -> name << "\t: " << row[j] << endl;
                    }
            cout << "Waktu  Squensial_Search: " << duration.count() << " milidetik" << endl;

                cout << "------------------------------------" << std::endl;
                }

                ++i;
            }

            if (!datanya_ketemu) {
                cout << "Data Tidak Ditemukan" << endl;
            }

            mysql_free_result(result); // Free the result set before executing another query
            return i; // untuk mengetahui iterasi

//pada kondisi ini untuk yang tidak berisifat unique data redundansi ( pengarang bisa memilki 2 buku)
        } else if (cari == "2" ||cari == "3" || cari == "4" || cari == "8" || cari == "9" || cari == "10") {
            pilihLower = toLower(pilih);
            int convert = stoi(cari);
            int i = 0;
            bool datanya_ketemu = false;
            std::string penerbitLower; // Deklarasi penerbitLower di sini
    auto start = chrono::high_resolution_clock::now();
            while ((row = mysql_fetch_row(result))) {
                // Jangan gunakan toLower untuk data penerbit dari MySQL
                std::string penerbit = (row[convert] != nullptr) ? row[convert] : "";

                // Bandingkan dengan input pencarian yang sudah diubah ke huruf kecil
                if (toLower(penerbit) == pilihLower) {
                    datanya_ketemu = true;
                    cout << "\nData ditemukan pada iterasi ke-" << i + 1 << ":\n";
                    iterasi_squensial = i + 1;
                    auto end = chrono::high_resolution_clock::now();
                    chrono::duration<double, milli> duration = end - start;
                     Waktu_squensial =duration.count();
                    // Tampilkan data
                    for (int j = 0; j < mysql_num_fields(result); ++j) {
                        cout << mysql_fetch_field_direct(result, j) -> name << ": " << row[j] << endl;
                    }
                cout << "Waktu  Squensial_Search: " << duration.count() << " milidetik" << endl;

                cout << "------------------------------------" << std::endl;
                }

                ++i;
            }

            if (!datanya_ketemu) {
                cout << "Data tidak ditemukan" << endl;
            }

            mysql_free_result(result);
            return i;

        }
    } else {
        cerr << "Query Error: " << mysql_error(conn) << endl;
    }

}




int Binary_search(MYSQL *conn, const std::vector<std::string> &sortedTitles, string cari, string pilih) {
    string pilihLower = toLower(pilih);
    string sortedTitleLower;
    int counter = 0;

    int qstate = 0;  // Inisialisasi qstate di luar blok if agar dapat digunakan di seluruh fungsi

    // Jika query untuk mendapatkan hasil binary search sukses
    if (qstate == 0) {
        // Jika cari adalah "1" atau "5"
        if (cari == "1" || cari == "5") {
            int i = 0;
            int j = sortedTitles.size() - 1;
            int k;
            bool datanya_ketemu = false;
           auto start = chrono::high_resolution_clock::now();
            while (!datanya_ketemu && i <= j) {
                counter++;
                k = (i + j) / 2;
                sortedTitleLower = toLower(sortedTitles[k]);

                if (sortedTitleLower == pilihLower) {
                    datanya_ketemu = true;

                } else {
                    if (sortedTitleLower > pilihLower) {
                        j = k - 1;
                    } else {
                        i = k + 1;
                    }
                }
            }


            // Menampilkan hasil binary search
            if (datanya_ketemu) {
                cout << "\nData ditemukan pada iterasi ke-" << counter+1 << "\n";
                iterasi_Binary = counter+1;
                                    auto end = chrono::high_resolution_clock::now();
                    chrono::duration<double, milli> duration = end - start;
//                    cout << "Waktu Binary_Search: " << duration.count() << " milidetik" << endl;
                    Waktu_Binary = duration.count();
                // Fetch additional details from the database based on the sorted title
                std::string query = "SELECT * FROM `data_buku1` WHERE Judul = '" + sortedTitles[k] + "'";
                const char *query_cstr = query.c_str();
                qstate = mysql_query(conn, query_cstr);  // Update nilai qstate

                if (qstate == 0) {
                    MYSQL_RES *result = mysql_store_result(conn);

                    if (result != nullptr) {
                        MYSQL_ROW row = mysql_fetch_row(result);

                        // Display the data
                        for (int j = 0; j < mysql_num_fields(result); ++j) {
                            cout << mysql_fetch_field_direct(result, j)->name << ": " << row[j] << endl;
                        }


                        // Bebaskan hasil query
                        mysql_free_result(result);
                    } else {
                        std::cerr << "Tidak dapat mengambil hasil query" << std::endl;
                    }
                } else {
                    std::cerr << "Query Error: " << mysql_error(conn) << std::endl;
                }

            } else {
                std::cout << "Data tidak ditemukan" << std::endl;
            }
        } else if (cari == "2" || cari == "3" || cari == "4" || cari == "8" || cari == "9" || cari == "10") {
            int i = 0;
            int j = sortedTitles.size() - 1;
            int k;
            bool datanya_ketemu = false;
        auto start = chrono::high_resolution_clock::now();
            while (!datanya_ketemu && i <= j) {
                 counter++;
                k = (i + j) / 2;
                sortedTitleLower = toLower(sortedTitles[k]);

                if (sortedTitleLower == pilihLower) {
                    datanya_ketemu = true;
                     auto end = chrono::high_resolution_clock::now();
                    chrono::duration<double, milli> duration = end - start;
                    cout << "Waktu eksekusi: " << duration.count() << " milidetik" << endl;
                    Waktu_Binary= duration.count() ;
                } else {
                    if (sortedTitleLower > pilihLower) {
                        j = k - 1;
                    } else {
                        i = k + 1;
                    }
                }
            }

            // Menampilkan hasil binary search
            if (datanya_ketemu) {
    std::cout << "Data ditemukan pada iterasi ke-" << counter+1 << ":" << std::endl;

iterasi_Binary = counter+1;

    string query = "SELECT * FROM `data_buku1` WHERE " + cariMap[cari] + " = '" + pilih + "'";
    const char *query_cstr = query.c_str();
    qstate = mysql_query(conn, query_cstr);

    if (qstate == 0) {
        MYSQL_RES *result = mysql_store_result(conn);

        if (result != nullptr) {
            // Display the data for all books matching the specified input
            while (MYSQL_ROW row = mysql_fetch_row(result)) {
                for (int j = 0; j < mysql_num_fields(result); ++j) {
                    cout << mysql_fetch_field_direct(result, j)->name << ": " << row[j] << endl;
                }
                cout << "------------------------------------" << std::endl;
            }

            // Bebaskan hasil query
            mysql_free_result(result);
        } else {
            std::cerr << "Tidak dapat mengambil hasil query" << std::endl;
        }
    } else {
        std::cerr << "Query Error: " << mysql_error(conn) << std::endl;
    }
} else {
    std::cout << "Data tidak ditemukan" << std::endl;
}
        }
    } else {
        std::cerr << "Query Error: " << mysql_error(conn) << std::endl;
    }

    return counter;
}

void Selection_Binary(MYSQL *conn, string cari, string pilih) {
    int convert = stoi(cari);
    int kapasitas= 10000;
string limitkapasitas = "LIMIT " + to_string(kapasitas);
    string query = "SELECT * FROM `data_buku1` " + limitkapasitas;
    const char* q = query.c_str();
    int qstate = mysql_query(conn, q);

    MYSQL_RES *result = mysql_store_result(conn);

    // Mendapatkan hasil query
    if (result != nullptr) {
        MYSQL_ROW row;
        int num_fields = mysql_num_fields(result);

        // Membuat array of strings untuk menyimpan judul buku
        std::vector<std::string> titles;

        // Menyimpan judul buku dari setiap baris hasil query
        while ((row = mysql_fetch_row(result))) {
            titles.push_back(row[convert] != nullptr ? row[convert] : "");
        }

        // Melakukan Selection Sort pada array of strings (judul buku)
        int n = titles.size();
        int minIndex;
        std::string temp;
auto start = chrono::high_resolution_clock::now();
        for (int i = 0; i < n - 1; i++) {
            minIndex = i;
            for (int j = i + 1; j < n; j++) {
                if (titles[j] < titles[minIndex]) {
                    minIndex = j;
                }
            }

            // Menukar posisi elemen pada array of strings
            temp = titles[i];
            titles[i] = titles[minIndex];
            titles[minIndex] = temp;
        }
        auto end = chrono::high_resolution_clock::now();
                    chrono::duration<double, milli> duration = end - start;

//     std::cout << "\nHasil Selection Sort : \n";
//    for (const auto &title : titles) {
//        std::cout << title << std::endl;
//       }

        // Menampilkan hasil Selection Sort
        cout << "\nHasil Binary Search : \n";
        cout << "Waktu Selection_sort: " << duration.count() << " milidetik" << endl;
        Binary_sort =duration.count();

        // Panggil fungsi Binary_search dengan data yang sudah diurutkan dan result set dari query
        Binary_search(conn, titles, cari, pilih);


        // Bebaskan hasil query
        mysql_free_result(result);
    } else {
        std::cerr << "Query Error: " << mysql_error(conn) << std::endl;
    }
}



string Search (MYSQL * conn, string cari){
    string pilih;
    std::string pilihLower;
    if (cari == "6") {
        cari = "8";

    } else if (cari == "7") {
        cari = "9";

    } else if (cari == "8") {
        cari = "10";

    }

        if (cari == "1") {
            cout << "\n=================================================";
            cout << "\n\t== Mencari Berdasarkan Judul == ";
            cout << "\n=================================================";
            cout << "\nMasukkan Judul yang ingin dicari: ";
            cin.ignore();
            getline(cin, pilih);
            Sequential_search(conn,cari,pilih);
            Selection_Binary (conn,cari,pilih);

        } else if (cari == "2") {
            cout << "\n=================================================";
            cout << "\n\t== Mencari Berdasarkan Pengarang == ";
            cout << "\n=================================================";
            cout << "\nMasukkan Pengarang yang ingin dicari: ";
            cin.ignore();
            getline(cin, pilih);
            Sequential_search(conn,cari,pilih);
          Selection_Binary (conn,cari,pilih);

        } else if (cari == "3") {
            cout << "\n=================================================";
            cout << "\n\t== Mencari Berdasarkan Penerbit == ";
            cout << "\n=================================================";
            cout << "\nMasukkan Penerbit yang ingin dicari: ";
            cin.ignore();
            getline(cin, pilih);
            Sequential_search(conn,cari,pilih);
            Selection_Binary (conn,cari,pilih);

        } else if (cari == "4") {
            cout << "\n=================================================";
            cout << "\n== Mencari Berdasarkan Tahun Terbit == ";
            cout << "\n=================================================";
            cout << "\nMasukkan Tahun Terbit yang ingin dicari: ";
            cin.ignore();
            getline(cin, pilih);
            Sequential_search(conn,cari,pilih);
            Selection_Binary (conn,cari,pilih);

        } else if (cari == "5") {
            cout << "\n=================================================";
            cout << "\n\t== Mencari Berdasarkan ISBN == ";
            cout << "\n=================================================";
            cout << "\nMasukkan ISBN yang ingin dicari: ";
            cin.ignore();
            getline(cin, pilih);
            Sequential_search(conn,cari,pilih);
            Selection_Binary (conn,cari,pilih);

        } else if (cari == "8") {
            cout << "\n=================================================";
            cout << "\n\t== Mencari Berdasarkan Rak == ";
            cout << "\n=================================================";
            cout << "\nMasukkan Rak yang ingin dicari: ";
            cin.ignore();
            getline(cin, pilih);
            Sequential_search(conn,cari,pilih);
            Selection_Binary (conn,cari,pilih);
        } else if (cari == "9") {
            cout << "\n=================================================";
            cout << "\n\t== Mencari Berdasarkan Genre == ";
            cout << "\n=================================================";
            cout << "\nMasukkan Genre yang ingin dicari: ";
            cin.ignore();
            getline(cin, pilih);
            Sequential_search(conn,cari,pilih);
            Selection_Binary (conn,cari,pilih);
        } else if (cari == "10") {
            cout << "\n=================================================";
            cout << "\n\t== Mencari Berdasarkan Status == ";
            cout << "\n=================================================";
            cout << "\nMasukkan Status yang ingin dicari: ";
            cin.ignore();
            getline(cin, pilih);
            pilihLower = toLower(pilih);
            Sequential_search(conn,cari,pilih);
            Selection_Binary (conn,cari,pilih);
        } else {
            cout << "Pilihan tidak valid\n";
         }

}


void Hasil_search() {
//Waktu_Binary= Binary_sort+Waktu_Binary;
    std::cout << std::endl << std::endl;
    std::cout << "+---------------------------------------+\n";
    std::cout << "|    Perbandingan Algoritma Searching   |\n";
    std::cout << "+------------------+--------------------+\n";
    std::cout << "|\e[0;30m\e[41m   Binary Search  \e[0m|\e[0;30m\e[46m  Squensial_Search  \e[0m|\n";
    std::cout << "+--------+---------+----------+---------+\n";
    std::cout << "|  Waktu | Iterasi |   Waktu  | Iterasi |\n";
    std::cout << "+------------------+--------------------+\n";
    std::cout << "| " << std::setw(6) << Waktu_Binary << " | " << std::setw(7) << iterasi_Binary << " | " << std::setw(8) << Waktu_squensial << " | " << std::setw(7) << iterasi_squensial << " |\n";
    std::cout << "+------------------+--------------------+\n";
    std::cout << "|     Algoritma Efisien Berdasarkan     |\n";
    std::cout << "+------------------+--------------------+\n";
    std::cout << "|       Waktu      |       Iterasi      |\n";
    std::cout << "+------------------+--------------------+\n";

    // Check for efficiency based on time
    if (Waktu_squensial >= Waktu_Binary) {
        std::cout << "| " << std::setw(12) << "\e[0;30m\e[41mBinary Search\e[0m" << "    |" << std::setw(22) << "|\n";
    } else if (Waktu_squensial <= Waktu_Binary) {
        std::cout << "| " << std::setw(12) << "\e[0;30m\e[46mSquential Search\e[0m" << " |" << std::setw(22) << "|\n";
    } else {
        std::cout << "| " << std::setw(16) << "Keduanya sama" << " |" << std::setw(19) << "|\n";
    }

    // Check for efficiency based on iterations
    if (iterasi_squensial >= iterasi_Binary) {
        std::cout << "| " << std::setw(18) << " |" << std::setw(29) << "\e[0;30m\e[41m Binary Search\e[0m" << "      |\n";
    } else if (iterasi_squensial <= iterasi_Binary) {
        std::cout << "| " << std::setw(18) << " |" << std::setw(20) << "\e[0;30m\e[46mSquential Search\e[0m" << "    |\n";
    } else {
        std::cout << "| " << std::setw(16) << "|" << std::setw(19) << "Keduanya sama" << " |\n";
    }

    std::cout << "+------------------+--------------------+\n";
}

void garis(){

           int panjang = 120;
    int ketebalan = 1;
    for (int i = 0; i < ketebalan; i++)
    {
        for (int j = 0; j < panjang; j++)
        {

            std::cout  << static_cast<char>(154);
        }

        std::cout << "\033[0m" << '\n';
    }
}

void header(){
cout<< endl;
    std::cout << "   ########  ######## ########  ########  ##     ##  ######  ########    ###    ##    ##    ###       ###    ##    ##\n";
    std::cout << "   ##     ## ##       ##     ## ##     ## ##     ## ##    ##    ##      ## ##   ##   ##    ## ##     ## ##   ###   ##\n";
    std::cout << "   ##     ## ##       ##     ## ##     ## ##     ## ##          ##     ##   ##  ##  ##    ##   ##   ##   ##  ####  ##\n";
    std::cout << "   ########  ######   ########  ########  ##     ##  ######     ##    ##     ## #####    ##     ## ##     ## ## ## ##\n";
    std::cout << "   ##        ##       ##   ##   ##        ##     ##       ##    ##    ######### ##  ##   ######### ######### ##  ####\n";
    std::cout << "   ##        ##       ##    ##  ##        ##     ## ##    ##    ##    ##     ## ##   ##  ##     ## ##     ## ##   ###\n";
    std::cout << "   ##        ######## ##     ## ##         #######   ######     ##    ##     ## ##    ## ##     ## ##     ## ##    ##\n\n";
    std::cout << "[]()()()()()()()()()()()()()()()()()()()()()()()()()()()()()()()()()()()()()()()()()()()()()()()()()()()()()()()()()()[]\n\n";
    std::cout << "                              ######## ######## ##    ## ##    ##  #######  ##     ##\n";
    std::cout << "                                 ##    ##       ##   ##  ##   ##  ##     ## ###   ###\n";
    std::cout << "                                 ##    ##       ##  ##   ##  ##   ##     ## #### ####\n";
    std::cout << "                                 ##    ######   #####    #####    ##     ## ## ### ##\n";
    std::cout << "                                 ##    ##       ##  ##   ##  ##   ##     ## ##     ##\n";
    std::cout << "                                 ##    ##       ##   ##  ##   ##  ##     ## ##     ##\n";
    std::cout << "                                 ##    ######## ##    ## ##    ##  #######  ##     ##\n\n";

garis();

}

// Fungsi untuk menampilkan progress bar
void showProgressBar(int progress, int total) {
    const int barWidth = 20;
    int level = (progress * 100) / total;

    int barsToShow = (level + 16) / 17; // Hitung berapa banyak tingkat yang harus ditampilkan

    std::cout << "\r[";

    for (int i = 0; i < barWidth; ++i) {
        if (i < barsToShow) {
            std::cout << "=";
        } else {
            std::cout << " ";
        }
    }

    std::cout << "] " << std::setw(3) << level << "%";
    std::cout.flush();
}

void simulateSorting(int total) {
    for (int i = 0; i <= total; ++i) {
        showProgressBar(i, total);

        // Jeda yang lebih singkat (misalnya, 1 milidetik)
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
    std::cout << std::endl;
}

void printMenu(const string& selectedOption) {

    system("cls");
     header();
      system("color D");
cout << endl<< endl<<endl<<endl;
    cout << (selectedOption == "Login" ? "\t\t\t\t\t       [X] " : "\t\t\t\t\t       [ ] ") << "Login" << " ";
    cout << (selectedOption == "Registrasi" ? "[X] " : "[ ] ") << "Registrasi" ;
    cout << endl<< endl<<endl<<endl;
    garis();
}


void menu2() {
cout << "\t\t\e[1;32m**Menu**\e[0m\n"
              << "\t\t\e[1;33m ___                            _        _                        \e[0m\n"
              << "\t\t\e[1;33m|  _ \\ ___ _ __ _ __  _   _ ___| |_ __ _| | ____ _  __ _ _ __     \e[0m\n"
              << "\t\t\e[1;33m| |_) / _ \\ '__| '_ \\| | | / __| __/ _` | |/ / _` |/ _` | '_ \    \e[0m\n"
              << "\t\t\e[1;33m|  __/  __/ |  | |_) | |_| \\__ \\ || (_| |   < (_| | (_| | | | |   \e[0m\n"
              << "\t\t\e[1;33m|_|   \\___|_|  | .__/ \\__,_|___/\\__\\__,_|_|\\_\\__,_|\\__,_|_| |_|   \e[0m\n"
              << "\t\t\e[1;33m               |_|                                                \e[0m\n"
              << "\t\t\e[1;32m   >>--> Selamat datang di perpustakaan Teknik Komputer <--<<      \e[0m\n";
}

void Header_Registrasi(){



    std::cout << "\e[1;92m"
              << "\t\t.########..########..######...####..######..########.########.....###.....######..####\n"
              << "\t\t.##.....##.##.......##....##...##..##....##....##....##.....##...##.##...##....##..##.\n"
              << "\t\t.##.....##.##.......##.........##..##..........##....##.....##..##...##..##........##.\n"
              << "\t\t.########..######...##...####..##...######.....##....########..##.....##..######...##.\n"
              << "\t\t.##...##...##.......##....##...##........##....##....##...##...#########.......##..##.\n"
              << "\t\t.##....##..##.......##....##...##..##....##....##....##....##..##.....##.##....##..##.\n"
              << "\t\t.##.....##.########..######...####..######.....##....##.....##.##.....##..######..####\n"
              << "\e[0m";  // Reset color

}

void Header_Login(){
    cout << "\e[1;92m";
   std::cout << "\t\t.########.########...#######..##.....##....##........#######...######...####.##....##" << std::endl;
    std::cout << "\t\t.##.......##.....##.##.....##.###...###....##.......##.....##.##....##...##..###...##" << std::endl;
    std::cout << "\t\t.##.......##.....##.##.....##.####.####....##.......##.....##.##.........##..####..##" << std::endl;
    std::cout << "\t\t.######...########..##.....##.##.###.##....##.......##.....##.##...####..##..##.##.##" << std::endl;
    std::cout << "\t\t.##.......##...##...##.....##.##.....##....##.......##.....##.##....##...##..##..####" << std::endl;
    std::cout << "\t\t.##.......##....##..##.....##.##.....##....##.......##.....##.##....##...##..##...###" << std::endl;
    std::cout << "\t\t.##.......##.....##..#######..##.....##....########..#######...######...####.##....##" << std::endl;
cout<< "\e[0m";  // Reset color


}


int printMenu2(int selectedOption) {
    char key;
    do {
        system("cls");
        // header(); // Assuming you have a function named 'header()' for displaying a header
        menu2();
        cout << "\n\e[1;97m\e[46m[::] Silahkan Pilih Menu [::]\e[0m ";
        cout << (selectedOption == 1 ? "\n [X] " : "\n [ ] ") << "Mengurutkan buku" << " ";
        cout << (selectedOption == 2 ? "\n [X] " : "\n [ ] ") << "Mencari buku" << " ";
        cout << (selectedOption == 3 ? "\n [X] " : "\n [ ] ") << "Menampilkan Buku";
        cout << endl << endl << endl << endl;

        key = _getch();
        switch (key) {
            case 72: // Up arrow key
                selectedOption = (selectedOption > 1) ? selectedOption - 1 : 3;
                break;
            case 80: // Down arrow key
                selectedOption = (selectedOption < 3) ? selectedOption + 1 : 1;
                break;
        }
    } while (key != 13);

    return selectedOption;  // Return the selected option after Enter is pressed
}


void Ascending (){
cout << "\n\t\t\t\e[1;33m >>>>>------> Ascending <------<<<<<\e[0m \n\n";
}
void Descending (){
cout << "\n\t\t\t\e[1;33m >>>>>------> Descending <------<<<<<\e[0m \n\n";
}

void bar(){
 string x = "\e[1;36m=\e[0m";

    for (int i = 60; i <= 100; i += 10) {
        // system("cls");

        cout << "\e[1;31m[\e[0m";

        int progress = ((i - 40) * 30) / 60;  // Menghitung panjang bagian terisi

        for (int j = 0; j < 30; ++j) {
            if (j < progress) {
                cout << x;
            } else {
                cout << " ";
            }
        }

        cout << "\e[1;31m]\e[0m \e[1;32m" << i << "%\e[0m" << endl;

        if (i < 100) {
            Sleep(3000);
        }
    }
}

void header_sorting(){
    const char* cyanColor = "\033[1;96m";  // Bright Cyan
    const char* blueColor = "\033[1;94m";  // Bright Blue
    const char* resetColor = "\033[0m";    // Reset color

    // Print the table with extra spacing
    std::cout << blueColor << "\t\t\t=============================" << resetColor << std::endl;
    std::cout << cyanColor << "\t\t\t| *** Algorithm Sorting *** |" << resetColor << std::endl;
    std::cout << blueColor << "\t\t\t=============================" << resetColor << std::endl;

}

int main() {
    char escKey;
        char q;
        int kapasitas;
    MYSQL* conn;
    conn = mysql_init(0);
    conn = mysql_real_connect(conn, "localhost", "root", "", "perpustakaan_str", 0, NULL, 0);
    string selectedOption = "Login";  // Opsi yang dipilih awal
if(conn){
    cout << "\nTerhubung ke Database\n";
    while (true) {
        printMenu(selectedOption);

        // Menerima input tombol panah
        int input = _getch();
        if (input == 224) {
            input = _getch();
            switch (input) {
                case 75:  // Tombol panah atas
                    if (selectedOption == "Login") {
                        selectedOption = "Registrasi";
                        system("cls");
                    } else if (selectedOption == "Registrasi") {
                        selectedOption = "Login";
                        system("cls");
                    }
                    break;
                case 77:  // Tombol panah bawah
                    if (selectedOption == "Login") {
                        selectedOption = "Registrasi";
                    } else if (selectedOption == "Registrasi") {
                        selectedOption = "Login";
                    }
                    break;
            }
        } else if (input == 13) { //enter
            if (selectedOption == "Login") {
                    system("cls");
Header_Login();
//Login(conn);
    string usm, pwd;
    bool berhasil = false;
    cout << endl<< endl;
    cout << "\n\e[1;97m\e[41m[::] Silahkan Login Terlebih Dahulu [::]\e[0m \n";
    cout << "Masukkan Username : "; cin >> usm;
    cout << "Masukkan Password : "; cin >> pwd;

   cout << "\n\033[46m"<< "\033[30m"<<"Verifikasi : ";
    char x= 219;
                    for (int i=0; i<25; i++)
                    {
                        cout << "\033[41m" << x << "\033[0m";
                        if(i<10)
                            Sleep(100);
                        if (i>=10&& i<20)
                            Sleep(100);
                        if (i>=10)
                            Sleep(25);
                    }

    if (Cek_Same_User(conn, usm, pwd) == 1) {
        cout << "Login berhasil!" << endl;
        berhasil =true;
    } else {
        cout << "\nLogin gagal. Username atau password salah." << endl;
        Sleep(10);
        cout << "\nKembali Kemenu\n";
        system ("pause") ;
        system ("cls") ;
        return main ();
    }




if (berhasil=true){
            s1:
            int pilih,pilih1;

            pilih=printMenu2(1);
            cout << "Masuk pilihan : ";
                    switch (pilih){
                    case 1: {
                            system("cls");
header_sorting();
                                std::cout << "\nMasukkan Banyak Data  : ";
                            cin >> kapasitas;

                            if (kapasitas >= 10000) {
                                //simulateSorting(kapasitas);
                                Ascending ();
                                cout << "\nWaiting For Sorting Ascending...\n";
                                bar();
                                Selection_sort(conn, kapasitas);
                                insertion_sort(conn, kapasitas);
                                Bubblesort(conn, kapasitas);
                                hasil_Sorting();

                                //simulateSorting(kapasitas);

                                // Descending
                                Descending();
                                cout << "\nWaiting For Sorting Descending...\n";
                                bar();
                                Selection_sort_Dsc(conn, kapasitas);
                                insertion_sort_dsc(conn, kapasitas);
                                Bubblesort_Dsc(conn, kapasitas);
                                hasil_Sorting();


                            } else {
                                 // Ascending
                                 Ascending ();
                                Selection_sort(conn, kapasitas);
                                insertion_sort(conn, kapasitas);
                                Bubblesort(conn, kapasitas);
                                hasil_Sorting();
                                ////

                                 // Descending
                                Descending();
                                Selection_sort_Dsc(conn, kapasitas);
                                insertion_sort_dsc(conn, kapasitas);
                                Bubblesort_Dsc(conn, kapasitas);
                                hasil_Sorting();


                            }

                            cout << "\n== Tekan [ESC] untuk Kembali ke[menu] ==\n";
                            escKey = _getch();
                            if (escKey == 27)
                            {
                                // Handle going back to menu 2
                                system("cls");
                                goto s1;

                            }
                        break;
                    }
            case 2:
{
    system("cls");
    string pilihstr;


cout << "\n=================================================";
cout << "\n\t   Mencari Buku Berdasarkan";
cout << "\n=================================================";
cout << "\n\e[1;37m\033[48;5;212m[::] Silahkan Pilih Menu [::]\e[0m ";
        cout << "\n  1.Judul\n  2.Pengarang\n  3.Penerbit\n  4.Tahun Terbit\n  5.ISBN\n  6.Rak\n  7.Genre\n  8.Status";
    cout << "\nPilih : ";
    cin >> pilihstr;
    system("cls");

    Search(conn, pilihstr);
    Hasil_search();
    cout << "\n== Tekan [ESC] untuk Kembali ke [menu] ==";
char escKey = _getch();
    escKey = _getch();
    if (escKey == 27)
    {
        // Handle going back to menu 2
        system("cls");
        goto s1;
    }



    // Continue with the rest of your code

    break;
}
            case 3 :{
                            system("cls");

                            tampil_data(conn);
                            cout << "\n== Tekan [ESC] untuk Kembali ke[menu] ==\n";
                            escKey = _getch();
                            if (escKey == 27)
                            {
                                // Handle going back to menu 2
                                system("cls");
                                goto s1;

                            }


                break;
                }
                    default:
                    {
                        // Tambahkan handling untuk nilai pilih yang tidak dikenali
                        cout << "Pilihan tidak valid\n";
                        break;
                    }
                    }
}else {
cout << "Login failed. Exiting program.\n";

}
            } else if (selectedOption == "Registrasi") {
                system("cls");
                Header_Registrasi();
    cout << endl<< endl;
    cout << "\n\e[1;97m\e[41m[::] Silahkan Registrasi Terlebih Dahulu [::]\e[0m \n";
                From_registrasi (conn, "Registrasi");
                system("pause");
                system("cls");
                return main();


            }
            break;
        }
    }
    }else{
        cout << "Tidak Terhubung ke Database";
    }

    return 0;
}





