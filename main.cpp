#include <iostream>
#include "struct.h"

const char filename[] = "students.bin";
FILE* buff;

void stud_flush(FILE* buffer, const char* filename)
{
    buffer = fopen(filename, "wb");
    fflush(buffer);
    fclose(buffer);
}

void swap(record &a, record &b)
{
    auto mirror = *&a;
    a = *&b;
    b = mirror;
}

// Sorting records by group
record* sort_by_groups(FILE* buffer, const char* filename, size_t size)
{
    buffer = fopen(filename, "rb");
    auto* array = new record[size];
    record structbuff{};

    for(size_t i = 0; fread(&structbuff, sizeof(record), 1, buffer) == 1; i++)
        array[i] = structbuff;

    for(size_t i = 0; i < size - 1; i++)
    {
        size_t min = i;
        for(size_t j = i + 1; j < size; j++)
        {
            if (strcmp(array[min].group, array[j].group) > 0)
                min = j;
        }
        swap(array[i], array[min]);
    }


    fclose(buffer);
    return array;
}

// Function to output faculty with the highest number of excellent students
void excellent_faculty(FILE* buffer, const char* filename, size_t size)
{
    buffer = fopen(filename, "rb");
    string faculties[size];
    size_t cur_fac = 0;
    short max = 0;
    record structbuffer{};
    bool found = false;

    short counter = 0;

    for(int i = 0; i < size; i++)
    {
        fseek(buffer, sizeof(record) * i, SEEK_SET);    // Setting cursor to certain position
        fread(&structbuffer, sizeof(record), 1, buffer);
        string seeked = structbuffer.faculty;       // Getting the first faculty to search
        bool res = false;
        for(size_t s = 0; s < cur_fac; s++)
            if(seeked == faculties[s])
                res = true;
        if(res) continue;   // Checking if this faculty is already in faculties array

        fseek(buffer, sizeof(record) * i, SEEK_SET);    // Setting cursor back after reading
        for(int j = i; fread(&structbuffer, sizeof(record), 1, buffer) == 1; j++)
        {
            if(strcmp(seeked.c_str(), structbuffer.faculty) == 0 && structbuffer.student.is_excellent())
            {
                counter++;
                found = true;
            }
        }
        if(max < counter)
        {
            max = counter;
            if(cur_fac > 0)
                for(int m = 1; m < cur_fac; m++)
                    faculties[m] = "";          // Checking if array was not empty and if we need to clear it

            faculties[0] = seeked;
            cur_fac = 1;
        }
        else if(max == counter && max != 0)                 // Checking if current faculty has same amount
        {                                                   // of excellent students as current best faculty
            faculties[cur_fac] = seeked;
            cur_fac++;
        }
        counter = 0;                                        // Resetting counter
    }

    if(found) {
        for (size_t i = 0; i < cur_fac; i++)
            cout << faculties[i] << " ";
        cout << "\n";
    }
    else
        cout << "Every faculty has losers :(\n";

    fclose(buffer);
}

// Function which outputs all the excellent students in the array
void honors_list(FILE* buffer, const char* filename)
{
    buffer = fopen(filename, "rb");
    record structbuffer{};
    bool found = false;

    for(int i = 0; fread(&structbuffer, sizeof(record), 1, buffer) == 1; i++)
    {
        if(structbuffer.student.is_excellent())
        {
            printf("Surname: %s;  Group: %s;  Year: %d;  Faculty: %s;\n",
                   structbuffer.student.surname,
                   structbuffer.group,
                   structbuffer.year,
                   structbuffer.faculty);
            found = true;
        }
    }
    if(!found)
        cout << "There are no excellent students :(\n";

    fclose(buffer);
}

// Function which outputs all the groups without backward students
void without_losers(FILE* buffer, const char* filename, size_t size)
{
    record* groups = sort_by_groups(buffer, filename, size);    // Getting array of sorted groups
    bool global_found = false;

    bool found = false;
    for(int i = 0; i < size + 1; i++)
    {
        if(i != 0 && strcmp(groups[i].group, groups[i-1].group) != 0)
        {
            if(!found)
            {
                cout << groups[i - 1].group << "\n";
                global_found = true;
            }
            found = false;
        }
        if(groups[i].student.is_dviechnik())
        {
            found = true;
            while(strcmp(groups[i].group, groups[i+1].group) == 0) i++;  // Shifting i to the index where the next group is
        }

    }

    if(!global_found)
        cout << "Every group has losers :(\n";
}



int main()
{
    stud_flush(buff, filename);

    record KPI[4] = {
            {{"Schevchenko", {5,5,1}}, "IASA", 1, "DA-12"},
            {{"Javelin", {5, 5, 5}}, "FBM", 1, "DA-12"},
            {{"Vovchok", {5, 5, 5}}, "IASA", 1, "DA-11"},
            {{"Melik", {5, 5, 5}}, "FBM", 1, "DA-12"}
    };

    write_fac(buff, filename, &KPI[0], 4);
    cout << "Initial array:\n";
    print_data(buff, filename);

    record data = {
            {"Kosenko", {5, 5, 5}}, "IASA", 1, "TI-14"
    };

    replace_data(buff, filename, &data, 2);
    cout << "\nArray after replacing 3rd element:\n";
    print_data(buff, filename);

    remove_data(buff, filename, 1);
    cout << "\nArray after removing 2nd element:\n";
    print_data(buff, filename);
    // TASK 1
    printf("\nFaculties with the most excellent students:\n");
    excellent_faculty(buff, filename, 3);
    // TASK 2
    printf("\nList of excellent students:\n");
    honors_list(buff, filename);
    // TASK 3
    printf("\nList of groups without losers:\n");
    without_losers(buff, filename, 3);

return 0;
}