/**
 * TOPIC 14 — File Handling and Serialization
 * ============================================
 * Covers: ifstream, ofstream, fstream, binary I/O,
 *         text I/O, file pointers, positioning,
 *         reading/writing structs, serialization concepts
 *
 * Compile (GCC 9+, Cygwin64 GCC 14):
 *   g++ -std=c++17 -Wall -Wno-unused-parameter 14_file_handling.cpp -o file_handling
 *
 * Compile (GCC 7/8 — filesystem was experimental):
 *   g++ -std=c++17 -Wall 14_file_handling.cpp -o file_handling -lstdc++fs
 *
 * NOTE: std::filesystem is in <filesystem> since C++17.
 *       GCC >= 9 links it automatically. GCC 7/8 needed -lstdc++fs.
 *       Cygwin64 GCC 14 (this workspace) needs no extra flag.
 */

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <cstring>
#include <filesystem>

namespace fs = std::filesystem;

// ── Text File — Writing ───────────────────────────────────────────────────────
void section_write_text() {
    std::cout << "\n=== Write Text File ===\n";

    // ofstream — output file stream
    // Modes: ios::out (default, truncate), ios::app (append), ios::trunc
    std::ofstream out("demo.txt");

    if (!out.is_open()) {
        std::cerr << "Failed to open demo.txt for writing\n";
        return;
    }

    out << "Line 1: Hello, File!\n";
    out << "Line 2: C++ file I/O\n";
    out << "Line 3: " << 42 << " " << 3.14 << "\n";

    // Explicit flush — ensures data is written to OS buffer
    out.flush();
    // out.close() is called automatically when out goes out of scope (RAII)

    std::cout << "demo.txt written\n";
}

// ── Text File — Reading ───────────────────────────────────────────────────────
void section_read_text() {
    std::cout << "\n=== Read Text File ===\n";

    std::ifstream in("demo.txt");
    if (!in) {   // operator bool — same as is_open()
        std::cerr << "Cannot open demo.txt\n";
        return;
    }

    // Read line by line
    std::string line;
    int linenum = 1;
    while (std::getline(in, line)) {
        std::cout << linenum++ << ": " << line << "\n";
    }

    // After exhausting a file, both eofbit AND failbit are set.
    // failbit alone (without eofbit) means a genuine read error.
    if (in.eof())
        std::cout << "Reached end of file\n";
    if (in.fail() && !in.eof())
        std::cerr << "Read error (genuine I/O failure)\n";
}

// ── Read Word by Word / Token by Token ────────────────────────────────────────
void section_read_tokens() {
    std::cout << "\n=== Read Tokens ===\n";

    std::ofstream tmpout("tokens.txt");
    tmpout << "apple 42 banana 3.14 cherry\n";
    tmpout.close();

    std::ifstream in("tokens.txt");

    std::string word;
    while (in >> word) {   // >> skips whitespace, reads until next whitespace
        std::cout << "token: '" << word << "'\n";
    }
}

// ── fstream — read and write to same file ────────────────────────────────────
void section_fstream() {
    std::cout << "\n=== fstream (read+write) ===\n";

    // Create a file first
    {
        std::ofstream out("rw_demo.txt");
        out << "HELLO WORLD\n";
    }

    // Open for both reading and writing
    std::fstream f("rw_demo.txt", std::ios::in | std::ios::out);
    if (!f) { std::cerr << "Cannot open rw_demo.txt\n"; return; }

    // Read current content
    std::string content;
    std::getline(f, content);
    std::cout << "Read: " << content << "\n";

    // Seek back to beginning and overwrite
    f.seekp(0, std::ios::beg);   // seekp = seek put (write position)
    f << "hello world";           // overwrite (lowercase)
    f.flush();

    // Seek back and read again
    f.seekg(0, std::ios::beg);   // seekg = seek get (read position)
    std::getline(f, content);
    std::cout << "After overwrite: " << content << "\n";
}

// ── File Positioning ─────────────────────────────────────────────────────────
void section_file_positioning() {
    std::cout << "\n=== File Positioning ===\n";

    // Write test file: exactly 10 characters, no newline
    {
        std::ofstream out("pos_demo.txt");
        out << "ABCDEFGHIJ";
    }

    std::ifstream in("pos_demo.txt");

    // tellg() — returns current read position (byte offset from start)
    std::cout << "Initial pos: " << in.tellg() << "\n";   // 0

    // seekg(offset, direction)
    //   ios::beg — offset from the beginning
    //   ios::cur — offset from current position (can be negative)
    //   ios::end — offset from end (usually negative)

    in.seekg(5, std::ios::beg);           // jump to byte 5
    std::cout << "After seekg(5,beg): " << in.tellg() << "\n";   // 5

    char c;
    in.get(c);                            // reads byte 5 → 'F', advances pos to 6
    std::cout << "char at pos 5: " << c << "\n";

    in.seekg(-3, std::ios::end);          // 3 bytes before end: pos = 10-3 = 7
    in.get(c);                            // reads byte 7 → 'H'
    std::cout << "3 from end: "   << c << "\n";

    in.seekg(1, std::ios::cur);           // 1 byte forward from current pos (8→9)
    in.get(c);                            // reads byte 9 → 'J'
    std::cout << "1 forward: "    << c << "\n";

    // Get file size: seek to end, read position
    in.seekg(0, std::ios::end);
    auto file_size = in.tellg();
    std::cout << "file size: " << file_size << " bytes\n";   // 10
}

// ── Binary File I/O ───────────────────────────────────────────────────────────
struct Employee {
    char   name[32];
    int    id;
    double salary;
};

void section_binary_io() {
    std::cout << "\n=== Binary File I/O ===\n";

    // Write binary
    {
        std::ofstream out("employees.bin", std::ios::binary);
        Employee employees[] = {
            {"Alice",  101, 75000.0},
            {"Bob",    102, 68000.0},
            {"Carol",  103, 82000.0}
        };

        int count = 3;
        // Write count first
        out.write(reinterpret_cast<const char*>(&count), sizeof(count));
        // Write all records
        out.write(reinterpret_cast<const char*>(employees), sizeof(employees));
        std::cout << "Written " << count << " employees to binary file\n";
    }

    // Read binary
    {
        std::ifstream in("employees.bin", std::ios::binary);

        int count;
        in.read(reinterpret_cast<char*>(&count), sizeof(count));
        std::cout << "Reading " << count << " employees:\n";

        for (int i = 0; i < count; ++i) {
            Employee emp;
            in.read(reinterpret_cast<char*>(&emp), sizeof(emp));
            std::cout << "  " << emp.name
                      << " id=" << emp.id
                      << " salary=$" << emp.salary << "\n";
        }
    }

    // NOTE: Binary serialization is platform-dependent (endianness, padding, alignment).
    // For cross-platform/cross-version serialization, use a library like
    // protobuf, flatbuffers, cereal, or nlohmann/json.
}

// ── Reading and Writing Structures (Text/CSV style) ───────────────────────────
struct Student {
    std::string name;
    int         id;
    double      gpa;

    // Serialize to CSV line
    std::string to_csv() const {
        return name + "," + std::to_string(id) + "," + std::to_string(gpa);
    }

    // Deserialize from CSV line
    static Student from_csv(const std::string& line) {
        Student s;
        std::istringstream ss(line);
        std::string token;
        std::getline(ss, s.name, ',');
        std::getline(ss, token, ','); s.id  = std::stoi(token);
        std::getline(ss, token, ','); s.gpa = std::stod(token);
        return s;
    }
};

void section_csv() {
    std::cout << "\n=== CSV Serialization ===\n";

    // Write CSV
    {
        std::ofstream out("students.csv");
        out << "name,id,gpa\n";   // header
        for (auto& s : {Student{"Alice",1,3.9}, Student{"Bob",2,3.5}, Student{"Carol",3,3.7}})
            out << s.to_csv() << "\n";
    }

    // Read CSV
    {
        std::ifstream in("students.csv");
        std::string line;
        std::getline(in, line);  // skip header
        std::cout << "Students:\n";
        while (std::getline(in, line)) {
            auto s = Student::from_csv(line);
            std::cout << "  " << s.name << " id=" << s.id << " gpa=" << s.gpa << "\n";
        }
    }
}

// ── String Streams ────────────────────────────────────────────────────────────
void section_string_streams() {
    std::cout << "\n=== String Streams ===\n";

    // ostringstream — build a string like writing to a file
    std::ostringstream oss;
    oss << "Value: " << 42 << " Pi: " << 3.14159 << " Flag: " << std::boolalpha << true;
    std::string result = oss.str();
    std::cout << result << "\n";

    // istringstream — parse a string like reading from a file
    std::string data = "10 20.5 hello 30";
    std::istringstream iss(data);
    int    a; double b; std::string c; int d;
    iss >> a >> b >> c >> d;
    std::cout << "parsed: " << a << " " << b << " " << c << " " << d << "\n";

    // stringstream — both read and write
    std::stringstream ss;
    ss << "2024-11-13";
    int year, month, day;
    char dash;
    ss >> year >> dash >> month >> dash >> day;
    std::cout << "date: year=" << year << " month=" << month << " day=" << day << "\n";
}

// ── Filesystem (C++17) ────────────────────────────────────────────────────────
void section_filesystem() {
    std::cout << "\n=== std::filesystem (C++17) ===\n";

    fs::path cwd = fs::current_path();
    std::cout << "Current path: " << cwd << "\n";

    // List files in current directory
    std::cout << "Files created by this program:\n";
    for (const auto& entry : fs::directory_iterator(cwd)) {
        if (entry.is_regular_file()) {
            auto ext = entry.path().extension().string();
            if (ext == ".txt" || ext == ".bin" || ext == ".csv") {
                std::cout << "  " << entry.path().filename()
                          << " (" << fs::file_size(entry) << " bytes)\n";
            }
        }
    }

    // Path operations
    fs::path p = "dir/subdir/file.txt";
    std::cout << "parent: "    << p.parent_path()           << "\n";
    std::cout << "filename: "  << p.filename()               << "\n";
    std::cout << "stem: "      << p.stem()                   << "\n";
    std::cout << "extension: " << p.extension()              << "\n";

    // Create and remove directory
    fs::path test_dir = "test_output_dir";
    fs::create_directory(test_dir);
    std::cout << "created: " << test_dir << " exists=" << fs::exists(test_dir) << "\n";
    fs::remove(test_dir);
    std::cout << "removed: " << test_dir << " exists=" << fs::exists(test_dir) << "\n";
}

// ── Cleanup helper ─────────────────────────────────────────────────────────────
void cleanup() {
    for (const auto& f : {"demo.txt","tokens.txt","rw_demo.txt",
                           "pos_demo.txt","employees.bin","students.csv"}) {
        std::error_code ec;
        fs::remove(f, ec);
    }
}

int main() {
    std::cout << "╔══ Topic 14: File Handling ══╗\n";

    section_write_text();
    section_read_text();
    section_read_tokens();
    section_fstream();
    section_file_positioning();
    section_binary_io();
    section_csv();
    section_string_streams();
    section_filesystem();

    cleanup();

    std::cout << "\n✓ Topic 14 complete.\n";
}
