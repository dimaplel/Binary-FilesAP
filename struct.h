#ifndef AP_14_STRUCT_H
#define AP_14_STRUCT_H

#include <iostream>
#include <cstring>

using std::cout, std::size_t, std::string,
std::fwrite, std::fread, std::fseek;

struct student
{
    char surname[15];
    short marks[3];

    bool is_excellent()
    {
        for(short mark : marks)
        {
            if (mark != 5) return false;
        }
        return true;
    }

    bool is_dviechnik()
    {
        for(short mark : marks)
        {
            if(mark <= 2) return true;
        }
        return false;
    }
};

struct record
{
    student student;
    char faculty[5];
    short year;
    char group[6];
};

// Function which writes record to binary file
void write_fac(FILE* buffer, const char* filename, record* faculty, size_t len)
{
    buffer = fopen(filename, "ab");

    for(size_t i = 0; i < len; i++)
    {
        fwrite(&faculty[i], sizeof(record), 1, buffer);
    }
    fclose(buffer);
}

// Function which replaces record under certain index in binary file
void replace_data(FILE* buffer, const char* filename, record* data, unsigned int fac_index)
{
    buffer = fopen(filename, "rb+");
    fseek(buffer, sizeof(record) * fac_index, SEEK_SET);    // Setting cursor position in file
    fwrite(data, sizeof(record), 1, buffer);
    fclose(buffer);
}

// Function which removes record under certain index from binary file
void remove_data(FILE* buffer, const char* filename, unsigned int fac_index)
{
    buffer = fopen(filename, "rb");
    char* tempfname = new char[strlen(filename) + 4];
    strcpy(tempfname, filename);
    strcat(tempfname, ".tmp");
    FILE* tempfile = fopen(tempfname, "wb");    // Creating another file for writing buffer

    bool if_found = false;

    record structbuffer{};

    for(int i = 0; fread(&structbuffer, sizeof(record), 1, buffer) == 1; i++)
    {
        if(i == fac_index)
        {
            cout << "Found record at [" << i << "] index!\n";                           // Copying records from binary
            if_found = true;                                                            // file unless it finds record
        }                                                                               // at the right place which will
        else fwrite(&structbuffer, sizeof(record), 1, tempfile);    // not be written to write buff
    }

    if(!if_found)
        cout << "Could not found record at [" << fac_index << "] index!\n";

    fclose(buffer);
    fclose(tempfile);

    remove(filename);
    rename(tempfname, filename);    // Renaming file to make the assigned record deleted in file
}

// Printing all records from binary file
void print_data(FILE* buffer, const char* filename)
{
    buffer = fopen(filename, "rb");
    size_t iter = 0;
    while(true)
    {
        record structbuffer{};
        if (fread(&structbuffer, sizeof(record), 1, buffer) != 1)
        {
            if (feof(buffer))
                printf("[LOG] EOF\n");
            else printf("[LOG] Error while reading binary\n");
            break;
        }
        printf("%d. Surname: %s;\tExam marks: %d, %d, %d;\tYear: %d;\tGroup: %s;\tFaculty: %s;\n",
               iter,
               structbuffer.student.surname,
               structbuffer.student.marks[0],
               structbuffer.student.marks[1],
               structbuffer.student.marks[2],
               structbuffer.year,
               structbuffer.group,
               structbuffer.faculty);
        iter++;
    }
    fclose(buffer);
}


#endif //AP_14_STRUCT_H