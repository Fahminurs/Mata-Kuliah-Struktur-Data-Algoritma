#include <iostream>
#include <mysql.h>
#include <chrono>
#include <ctime>
#include <sstream>
#include <vector>
#include <iomanip>
#include <numeric>
#include <algorithm> // for std::transform
#include <cctype>    // for std::tolower
#include <map>

using namespace std;
//role 1 = user
//role 0=Admin

int qstate = 0;
string role;


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


int Cek_Same_User(MYSQL *conn, string usm, string pwd) {
    // Check if the username and password already exist
    stringstream checkQuery;
    checkQuery << "SELECT COUNT(*) FROM tbl_user WHERE username = '" << usm << "' AND password = '" << pwd << "';";
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


    cin.ignore(numeric_limits<streamsize>::max(), '\n');
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
    ss << "INSERT INTO tbl_user (id_user, nama, email, username, password, no_tlp, role, date_created) VALUES (" <<
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
if(pil_reg != "Registrasi" ){
    char Y;
    cout << "Apakah Ingin Menambahkan Akun Lagi <Y/N> ? : ";
    cin >> Y;
    if (Y == 'y' || Y == 'Y') {
        From_registrasi(conn, pil_reg);
    } else {
       cout << "Kembali Ke Menu";
    }
}else{

cout << "Kembali Ke Menu";
}
}else {
        cout << "Data Tidak Berhasil Ditambahkan" << endl;
        cerr << "Error: " << mysql_error(conn) << endl;
    }
}else{
cout << "Username dan password sudah terdaftar. Silakan gunakan yang lain." << endl;
}
}


void tampil_data(MYSQL* conn) {
    const char* query = "SELECT * FROM `data_buku` WHERE 1";
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




void Bubblesort(MYSQL* conn) {
    const char* query = "SELECT * FROM `data_buku`";
    int qstate = mysql_query(conn, query);

    if (qstate == 0) {
        MYSQL_RES* result = mysql_store_result(conn);

        if (result != nullptr) {
            MYSQL_ROW row;
            vector<vector<string>> rowData;  // Vector of vectors to store all columns

            // Fetch and store data from the database
            while ((row = mysql_fetch_row(result))) {
                vector<string> currentRow;  // Vector to store data for the current row

                // Fetch the first 10 columns of the row
                for (int i = 1; i <= 10; ++i) {
                    if (row[i] != nullptr) {
                        currentRow.push_back(row[i]);
                    } else {
                        currentRow.push_back(""); // Handle NULL values
                    }
                }

                rowData.push_back(currentRow);  // Add the current row to the vector of rows
            }

            // Bubble Sort for the first column (Judul)
            int n = rowData.size();
            bool tukar;

            do {
                tukar = false;

                for (int i = 0; i < n - 1; i++) {
                    // Modify the comparison logic here for the first column (Judul)
                    if (rowData[i][0] > rowData[i + 1][0]) {
                        swap(rowData[i], rowData[i + 1]);
                        tukar = true;
                    }
                }

            } while (tukar);

            // Display the sorted data for the first column (Judul)
            cout << "\nData setelah diurutkan berdasarkan Judul (Bubble Sort):\n";
            for (int i = 0; i < n; i++) {
                for (int j = 0; j < 10; ++j) {
                    cout << rowData[i][j] << "\t";
                }
                cout << endl;
            }

            mysql_free_result(result);
        } else {
            cerr << "Error: " << mysql_error(conn) << endl;
        }
    } else {
        cerr << "Query Error: " << mysql_error(conn) << endl;
    }
}

void Selection_sort(MYSQL* conn) {
    const char* query = "SELECT * FROM `data_buku`";
    int qstate = mysql_query(conn, query);

    if (qstate == 0) {
        MYSQL_RES* result = mysql_store_result(conn);

        if (result != nullptr) {
            MYSQL_ROW row;
            vector<vector<string>> data;

            // Fetch and store data from the database
            while ((row = mysql_fetch_row(result))) {
                vector<string> rowData;
                for (int i = 1; i < 2; ++i) {  // Start the loop from 0
                    if (row[i] != nullptr) {
                        rowData.push_back(row[i]);
                    } else {
                        rowData.push_back(""); // Handle NULL values
                    }
                }
                data.push_back(rowData);
            }

            // Sorting using Selection Sort for the first column (Judul)
            int n = data.size();
            for (int i = 0; i < n - 1; i++) {
                int minIndex = i;
                for (int j = i + 1; j < n; j++) {  // Change the loop condition to < n
                    // Modify the comparison logic here based on the data type
                    if (data[j][0] < data[minIndex][0]) {  // Compare with the first column (Judul)
                        minIndex = j;
                    }
                }

                // Swap the found minimum element with the first element
                if (minIndex != i) {
                    swap(data[i], data[minIndex]);
                }
            }

            // Display the sorted data for the first column (Judul)
            cout << "\nData setelah diurutkan berdasarkan Judul (Selection Sort):\n"; //untuk menampilkan hasil
            for (int i = 0; i < n; i++) {
                for (const auto& item : data[i]) {
                    cout << item << "\t";
                }
                cout << endl;
            }

            mysql_free_result(result);
        } else {
            cerr << "Error: " << mysql_error(conn) << endl;
        }
    } else {
        cerr << "Query Error: " << mysql_error(conn) << endl;
    }
}

void insertion_sort(MYSQL* conn) {
    const char* query = "SELECT * FROM `data_buku`";
    int qstate = mysql_query(conn, query);

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

            // Sorting using Insertion Sort for the first column (Judul)
            int n = data.size();
            for (int i = 1; i < n; i++) {
                vector<string> data_sisip = data[i];
                int j = i - 1;

                while (j >= 0 && data[j][0] < data_sisip[0]) {
                    data[j + 1] = data[j];
                    j = j - 1;
                }

                data[j + 1] = data_sisip;
            }

            // Display the sorted data for the first column (Judul)
            cout << "\nData setelah diurutkan berdasarkan Judul (Insertion Sort):\n";
            for (int i = 0; i < n; i++) {
                for (const auto& item : data[i]) {
                    cout << item << "\t";
                }
                cout << endl;
            }

            mysql_free_result(result);
        } else {
            cerr << "Error: " << mysql_error(conn) << endl;
        }
    } else {
        cerr << "Query Error: " << mysql_error(conn) << endl;
    }
}

std::string toLower(const std::string& str) {
    std::string result = str;
    transform(result.begin(), result.end(), result.begin(),
                   [](unsigned char c) { return std::tolower(c); });
    return result;
}





int Sequential_search(MYSQL * conn, string cari, string pilih) {

//cout << "\nIni cari " << cari << endl;
//cout << "\nIni PIlih " << pilih << endl;




    const char * query = "SELECT * FROM `data_buku`";
    int qstate = mysql_query(conn, query);
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
            while ((row = mysql_fetch_row(result)) && !datanya_ketemu) {
                //cout << "Iterasi : " << i + 1 << endl;

                // Convert database title to lowercase
                std::string titleLower = (row[convert] != nullptr) ? toLower(row[convert]) : "";

                if (titleLower == pilihLower) {
                    datanya_ketemu = true;
                    cout << "\nData ditemukan pada iterasi ke-" << i + 1 << ":\n";
                    // Display the data or perform any other actions
                    for (int j = 0; j < mysql_num_fields(result); ++j) {
                        cout << mysql_fetch_field_direct(result, j) -> name << ": " << row[j] << endl;
                    }
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

            while ((row = mysql_fetch_row(result))) {
                // Jangan gunakan toLower untuk data penerbit dari MySQL
                std::string penerbit = (row[convert] != nullptr) ? row[convert] : "";

                // Bandingkan dengan input pencarian yang sudah diubah ke huruf kecil
                if (toLower(penerbit) == pilihLower) {
                    datanya_ketemu = true;
                    cout << "\nData ditemukan pada iterasi ke-" << i + 1 << ":\n";

                    // Tampilkan data
                    for (int j = 0; j < mysql_num_fields(result); ++j) {
                        cout << mysql_fetch_field_direct(result, j) -> name << ": " << row[j] << endl;
                    }
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

    cout << "Pilih pada binary search: " << pilih << endl;
    cout << "Cari pada binary search: " << cari << endl;

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
                std::cout << "Data ditemukan" << std::endl;
                std::cout << "Data ditemukan :::: " << sortedTitleLower << endl;
                cout << "\n\nData ditemukan pada iterasi ke-" << counter << ":\n";
                cout << "\nData " << sortedTitles[k] << ":\n\n";

                // Fetch additional details from the database based on the sorted title
                std::string query = "SELECT * FROM `data_buku` WHERE Judul = '" + sortedTitles[k] + "'";
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

            while (!datanya_ketemu && i <= j) {
                cout << "iterasi ke- " << counter+1<< ":" << std::endl;
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
    std::cout << "Data ditemukan pada iterasi ke-" << counter << ":" << std::endl;

    // Fetch additional details from the database based on the user input
    string query = "SELECT * FROM `data_buku` WHERE " + cariMap[cari] + " = '" + pilih + "'";
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
    const char *query = "SELECT * FROM `data_buku`";
    int qstate = mysql_query(conn, query);
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

     std::cout << "\nHasil Selection Sort : \n";
    for (const auto &title : titles) {
        std::cout << title << std::endl;
       }

        // Menampilkan hasil Selection Sort
        cout << "\nHasil Binary Search : \n";

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
            cout << "\n== Mencari Berdasarkan Judul == ";
            cout << "\nMasukkan Judul yang ingin dicari: ";
            cin.ignore();
            getline(cin, pilih);
            Sequential_search(conn,cari,pilih);
            Selection_Binary (conn,cari,pilih);

        } else if (cari == "2") {
            cout << "\n== Mencari Berdasarkan Pengarang == ";
            cout << "\nMasukkan Pengarang yang ingin dicari: ";
            cin.ignore();
            getline(cin, pilih);
            Sequential_search(conn,cari,pilih);
          Selection_Binary (conn,cari,pilih);

        } else if (cari == "3") {
            cout << "\n== Mencari Berdasarkan Penerbit == ";
            cout << "\nMasukkan Penerbit yang ingin dicari: ";
            cin.ignore();
            getline(cin, pilih);
            Sequential_search(conn,cari,pilih);
            Selection_Binary (conn,cari,pilih);

        } else if (cari == "4") {
            cout << "\n== Mencari Berdasarkan Tahun Terbit == ";
            cout << "\nMasukkan Tahun Terbit yang ingin dicari: ";
            cin.ignore();
            getline(cin, pilih);
            Sequential_search(conn,cari,pilih);
            Selection_Binary (conn,cari,pilih);

        } else if (cari == "5") {
            cout << "\n== Mencari Berdasarkan ISBN == ";
            cout << "\nMasukkan ISBN yang ingin dicari: ";
            cin.ignore();
            getline(cin, pilih);
            Sequential_search(conn,cari,pilih);
            Selection_Binary (conn,cari,pilih);

        } else if (cari == "8") {
            cout << "\n== Mencari Berdasarkan Rak == ";
            cout << "\nMasukkan Rak yang ingin dicari: ";
            cin.ignore();
            getline(cin, pilih);
            Sequential_search(conn,cari,pilih);
            Selection_Binary (conn,cari,pilih);
        } else if (cari == "9") {
            cout << "\n== Mencari Berdasarkan Genre == ";
            cout << "\nMasukkan Genre yang ingin dicari: ";
            cin.ignore();
            getline(cin, pilih);
            Sequential_search(conn,cari,pilih);
            Selection_Binary (conn,cari,pilih);
        } else if (cari == "10") {
            cout << "\n== Mencari Berdasarkan Status == ";
            cout << "\nMasukkan Status yang ingin dicari: ";
            cin.ignore();
            getline(cin, pilih);
            pilihLower = toLower(pilih);
            Sequential_search(conn,cari,pilih);
        } else {
            cout << "Pilihan tidak valid\n";
            // Handle invalid option (optional)
        }

}




int main(){
    MYSQL* conn;
    conn = mysql_init(0);
    conn = mysql_real_connect(conn, "localhost", "root", "", "perpustakaan_str", 0, NULL, 0);


    if(conn){
        int pilih,pilih1;
        cout << "Terhubung ke Database";
 //       cout << "\n1.Login\n2.Registrasi\n"; cin>> pilih;
  //      if(pilih==1){
       // From_login(conn);
        if ("admin") {
                cout << role;
            cout << "Admin\n";
            cout << "1. Mengurutkan buku\n2.Mencari buku\n3.Menampilkan Buku\n4.Update Buku\n5. Akun Admin";
            cout << "Masuk pilihan\n";
            cin >> pilih;
                    switch (pilih){
//                        Admin
                    case 1:
                    {
                    insertion_sort (conn);
                    Bubblesort(conn);
                    Selection_sort (conn);
                        break;
                    }
            case 2:
                {
                    //Selection_Binary(conn);
                    string pilihstr;
                    char q;
                    cout << "Cari Buku Berdasarkan :\n1.Judul\n2.Pengarang\n3.Penerbit\n4.Tahun Terbit\n5.ISBN\n6.Rak\n7.Genre\n8.Status";
                    cout << "\nPilih : ";
                    cin >> pilihstr;

                   Search (conn,pilihstr);
                    //Sequential_search(conn, pilihstr);

                    cout << "Apakah anda ingin mencari lagi <Y/y/N/n>? :";cin >> q;
                    if(q == 'Y' || q == 'y'){
                            system("cls");
                        return main ();


                    }else if (q == 'N' || q == 'n'){

                    return 0;
                    }


                    break;
                }
                    case 3:
                    {
                        cout << "Hapus Buku\n";
                        // Tambahkan kode untuk menangani opsi "Hapus Buku" di sini
                        break;
                    }
                    case 4:
                    {
                        cout << "Update Buku\n";
                        // Tambahkan kode untuk menangani opsi "Update Buku" di sini
                        break;
                    }
                    case 5:
                    {
                        string Pil_Reg;
                        cout << "Tambah Akun\n";
                        cout << "Tambahkan Untuk Admin/User : "; cin >> Pil_Reg;
                        if(Pil_Reg=="admin" || Pil_Reg=="user"  ){
                        From_registrasi(conn,Pil_Reg);
                        return main();
                        }else{

                        cout << "Belummm";
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
                    }else{
                    cout << "Loginya Sebagai User";



                }
   // }else if(pilih==2){
   //     From_registrasi(conn,"Registrasi");
      //  system ("pause");
       // return main();
   // }else{
      //  cout << "Tidak Ada Pilihan" << endl;
   // }




    }else{
        cout << "Tidak Terhubung ke Database";
    }
}
