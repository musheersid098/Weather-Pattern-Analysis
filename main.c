#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_ROWS 4000
#define MAX_LINE_CHARS 1000
#define DATA_COLS 8
#define CENTURIES_COUNT 4
#define MONTHS_COUNT 12

double read_next_token(char *temp) {
  temp = strtok(NULL, ",");
  if (temp == NULL) {
    return -1; // Error
  } else {
    return (atof(temp));
  }
}

void convert_date(int *year, int *month, int *day, char date[10]) {

  char *saveptr;
  char *token = strtok_r(date, "-", &saveptr);

  // Retrieve year
  if (token == NULL)
    printf("Date format is wrong");
  *year = atoi(token);
  token = strtok_r(NULL, "-", &saveptr);

  // Retrieve month
  if (token == NULL)
    printf("Date format is wrong");
  *month = atoi(token);
  token = strtok_r(NULL, "-", &saveptr);

  // Retrieve day
  if (token == NULL)
    printf("Date format is wrong");
  *day = atoi(token);
}

void read_data(FILE *csv_file, int data_dates[MAX_ROWS][3],
               double data_temps[DATA_COLS][MAX_ROWS], int *data_length) {
  char line[MAX_LINE_CHARS];

  // Read the first line
  fgets(line, sizeof(line), csv_file);

  // Read the rest of the lines
  int index = 0;
  while (fgets(line, sizeof(line), csv_file)) {
    // Declare temporary variable to store values read each value in the line
    char *token;

    // Read date from the line
    token = strtok(line, ",");
    char *date = token;

    // Get year, month and day as an integer
    int year, month, day;
    convert_date(&year, &month, &day, date);
    data_dates[index][0] = year;
    data_dates[index][1] = month;
    data_dates[index][2] = day;

    // Read first two values
    data_temps[0][index] = read_next_token(token);
    data_temps[1][index] = read_next_token(token);

    // If no data provided, skip this iteration
    if (data_temps[0][index] == -1 || data_temps[1][index] == -1) {
      continue;
    }

    // If year is greator or equal to 1850, read the rest of the values
    if (year >= 1850) {
      data_temps[2][index] = read_next_token(token);
      data_temps[3][index] = read_next_token(token);
      data_temps[4][index] = read_next_token(token);
      data_temps[5][index] = read_next_token(token);
      data_temps[6][index] = read_next_token(token);
      data_temps[7][index] = read_next_token(token);
    }

    *data_length += 1;
    index += 1;
  }
}

// Development use only
void print_data(int data_dates[MAX_ROWS][3],
                double data_temps[DATA_COLS][MAX_ROWS], int data_length) {
  printf("%d", data_length);

  for (int i = 0; i < data_length; i++) {
    // Print formatted date
    printf("%d-%d-%d ", data_dates[i][0], data_dates[i][1], data_dates[i][2]);

    // Print temperature data
    printf("%.3lf %.3lf %.3lf %.3lf %.3lf %.3lf %.3lf %.3lf", data_temps[0][i],
           data_temps[1][i], data_temps[2][i], data_temps[3][i],
           data_temps[4][i], data_temps[5][i], data_temps[6][i],
           data_temps[7][i]);

    // Skip line
    printf("\n");
  }
}

void calc_yearly_avgs(double data_temps[DATA_COLS][MAX_ROWS],
                      int data_dates[MAX_ROWS][3], int data_length,
                      double yearly_avgs[DATA_COLS][MAX_ROWS],
                      int yearly_avgs_date[DATA_COLS][MAX_ROWS][3],
                      int yearly_avgs_length[DATA_COLS]) {
  double yearly_avgs_sum[DATA_COLS] = {0};

  for (int i = 0; i < DATA_COLS; i++) {
    for (int j = 0; j < data_length; j++) {
      int year = data_dates[j][0], month = data_dates[j][1];

      if (year >= 1760 && year <= 2015) {

        // Set the date of the current yearly average
        if (month == 1) {
          yearly_avgs_date[i][yearly_avgs_length[i]][0] = data_dates[j][0];
          yearly_avgs_date[i][yearly_avgs_length[i]][1] = data_dates[j][1];
          yearly_avgs_date[i][yearly_avgs_length[i]][2] = data_dates[j][2];
        }

        // Add current temp to sum
        yearly_avgs_sum[i] += data_temps[i][j];

        // Calculate the current yearly average if end of year
        if (month == 12) {
          yearly_avgs[i][yearly_avgs_length[i]] = yearly_avgs_sum[i] / 12;
          yearly_avgs_length[i] += 1;
          yearly_avgs_sum[i] = 0;
        }
      }
    }
  }
}

void solve_problem_1(int data_dates[MAX_ROWS][3], double data_temps[MAX_ROWS],
                     int data_length, int yearly_land_avgs_date[MAX_ROWS][3],
                     double yearly_land_avgs[MAX_ROWS],
                     int *yearly_land_avgs_length) {
  double yearly_sums[MAX_ROWS] = {0};

  for (int i = 0; i < data_length; i++) {
    int year = data_dates[i][0], month = data_dates[i][1];

    // Collect averages for years greater than 1760
    if (year >= 1760 && year <= 2015) {
      // Set the year of the current average
      if (month == 1) {
        yearly_land_avgs_date[*yearly_land_avgs_length][0] = data_dates[i][0];
        yearly_land_avgs_date[*yearly_land_avgs_length][1] = data_dates[i][1];
        yearly_land_avgs_date[*yearly_land_avgs_length][2] = data_dates[i][2];
      }

      // Add current average to the total sum
      yearly_sums[*yearly_land_avgs_length] += data_temps[i];

      // If it is the last month of the year, calculate the average
      if (month == 12) {
        yearly_land_avgs[*yearly_land_avgs_length] =
            yearly_sums[*yearly_land_avgs_length] / 12;
        *yearly_land_avgs_length += 1;
      }
    }
  }

  // Print data
  printf("PROBLEM 1\n");
  printf("|%-15s|%-35s|\n", "Year", "Yearly Average Land Temperature");
  for (int i = 0; i < *yearly_land_avgs_length; i++) {
    printf("|%-15d|%-35lf|\n", yearly_land_avgs_date[i][0],
           yearly_land_avgs[i]);
  }
  printf("\n");
}

void solve_problem_2(int data_dates[MAX_ROWS][3],
                     double data_land_temps[MAX_ROWS], int data_length,
                     double century_land_avgs[CENTURIES_COUNT],    // output
                     int century_land_avgs_length[CENTURIES_COUNT] // output
) {
  double century_sums[CENTURIES_COUNT] = {0};
  int centuries_start[CENTURIES_COUNT] = {1760, 1800, 1900, 2000},
      centuries_end[CENTURIES_COUNT] = {1799, 1899, 1999, 2015};

  for (int i = 0; i < data_length; i++) {
    int year = data_dates[i][0], month = data_dates[i][1];

    // For every century
    for (int j = 0; j < CENTURIES_COUNT; j++) {

      // If year is in the current century
      if (year >= centuries_start[j] && year <= centuries_end[j]) {
        // Add to total sum
        century_sums[j] += data_land_temps[i];
        century_land_avgs_length[j] += 1;

        // Calculate averages if last month of last year
        if (year == centuries_end[j] && month == 12) {
          century_land_avgs[j] = century_sums[j] / century_land_avgs_length[j];
        }
      }
    }
  }

  // Print data
  printf("PROBLEM 2\n");
  printf("|%-20s|%-35s|\n", "Century", "Century Average Land Temperature");
  for (int i = 0; i < CENTURIES_COUNT; i++) {
    printf("|%d-%-15d|%-35lf|\n", centuries_start[i], centuries_end[i],
           century_land_avgs[i]);
  }
  printf("\n");
}

void solve_problem_3(int data_dates[MAX_ROWS][3],
                     double data_land_temps[MAX_ROWS], int data_length,
                     double monthly_avgs[MONTHS_COUNT],
                     int monthly_avgs_length[MONTHS_COUNT]) {
  double monthly_sums[MONTHS_COUNT] = {0};

  for (int i = 0; i < data_length; i++) {
    int year = data_dates[i][0], month = data_dates[i][1];
    int index = month - 1;

    if (year >= 1900 && year <= 2015) {
      // Add current month's land temp to the current month's total sum
      monthly_sums[index] += data_land_temps[i];
      monthly_avgs_length[index] += 1;

      // Calculate averages if it is the last year of the dataset
      if (year == 2015) {
        monthly_avgs[index] = monthly_sums[index] / monthly_avgs_length[index];
      }
    }
  }

  // Print data
  printf("PROBLEM 3\n");
  printf("|%-15s|%-35s|\n", "Month", "Month's Average Land Temperature");
  for (int i = 0; i < MONTHS_COUNT; i++) {
    printf("|%-15d|%-35lf|\n", i + 1, monthly_avgs[i]);
  }
  printf("\n");
}

void solve_problem_4(int data_dates[MAX_ROWS][3],
                     double data_land_temps[MAX_ROWS], int data_length,
                     int hottest_temp_date[3], int coldest_temp_date[3],
                     double *hottest_temp, double *coldest_temp) {
  for (int i = 0; i < data_length; i++) {
    double current_temp = data_land_temps[i];

    // If first month or current temp is hotter than hottest
    if (i == 0 || current_temp > *hottest_temp) {
      // Set its date
      hottest_temp_date[0] = data_dates[i][0];
      hottest_temp_date[1] = data_dates[i][1];
      hottest_temp_date[2] = data_dates[i][2];

      // Set its value
      *hottest_temp = current_temp;
    }

    // If first month or current temp is colder than coldest
    if (i == 0 || current_temp < *coldest_temp) {
      // Set its date
      coldest_temp_date[0] = data_dates[i][0];
      coldest_temp_date[1] = data_dates[i][1];
      coldest_temp_date[2] = data_dates[i][2];

      // Set its value
      *coldest_temp = current_temp;
    }
  }

  // Print data
  printf("PROBLEM 4\n");
  printf("The hottest month occured on %d-%d-%d with a temperature of %lf\n",
         hottest_temp_date[0], hottest_temp_date[1], hottest_temp_date[2],
         *hottest_temp);
  printf("The coldest month occured on %d-%d-%d with a temperature of %lf\n",
         coldest_temp_date[0], coldest_temp_date[1], coldest_temp_date[2],
         *coldest_temp);
  printf("\n");
}

void solve_problem_5(double yearly_land_avgs[MAX_ROWS],
                     int yearly_land_avgs_date[MAX_ROWS][3],
                     int yearly_land_avgs_length,
                     int hottest_yearly_avg_date[3],
                     int coldest_yearly_avg_date[3], double *hottest_yearly_avg,
                     double *coldest_yearly_avg) {
  for (int i = 0; i < yearly_land_avgs_length; i++) {
    double current_avg = yearly_land_avgs[i];

    // If first month, or current avg is hotter than current hottest
    if (i == 0 || current_avg > *hottest_yearly_avg) {
      // Set its date
      hottest_yearly_avg_date[0] = yearly_land_avgs_date[i][0];
      hottest_yearly_avg_date[1] = yearly_land_avgs_date[i][1];
      hottest_yearly_avg_date[2] = yearly_land_avgs_date[i][2];

      // Set its value
      *hottest_yearly_avg = current_avg;
    }

    // If first month, or current avg is colder than current coldest
    if (i == 0 || current_avg < *coldest_yearly_avg) {
      // Set its date
      coldest_yearly_avg_date[0] = yearly_land_avgs_date[i][0];
      coldest_yearly_avg_date[1] = yearly_land_avgs_date[i][1];
      coldest_yearly_avg_date[2] = yearly_land_avgs_date[i][2];

      // Set its value
      *coldest_yearly_avg = current_avg;
    }
  }

  // Print data
  printf("PROBLEM 5\n");
  printf("The hottest yearly average temperature occured on %d with a "
         "temperature of %lf\n",
         coldest_yearly_avg_date[0], *hottest_yearly_avg);
  printf("The coldest yearly average temperature occured on %d with a "
         "temperature of %lf\n",
         coldest_yearly_avg_date[0], *coldest_yearly_avg);
  printf("\n");
}

void solve_problem_6(FILE *problem_6_plot, double yearly_land_avgs[MAX_ROWS],
                     int yearly_land_avgs_date[MAX_ROWS][3],
                     int yearly_land_avgs_length) {
  // Print Header
  fprintf(problem_6_plot, "# Year AverageTemperature\n");

  // Print data on each line
  for (int i = 0; i < yearly_land_avgs_length; i++) {
    int year = yearly_land_avgs_date[i][0];

    if (year >= 1760 && year <= 2015) {
      fprintf(problem_6_plot, "%d %lf\n", yearly_land_avgs_date[i][0],
              yearly_land_avgs[i]);
    }
  }
}

void solve_problem_7(FILE *problem_7_plot, double yearly_land_avgs[MAX_ROWS],
                     int yearly_land_avgs_date[MAX_ROWS][3],
                     int yearly_land_avgs_length) {
  // Print Header
  fprintf(problem_7_plot, "# YearsAfter Average19thCenturyTemperature "
                          "Average20thCenturyTemperature\n");

  // Print data on each line
  for (int i = 0; i < yearly_land_avgs_length; i++) {
    int year = yearly_land_avgs_date[i][0];

    if (year >= 1800 && year < 1900) {
      fprintf(problem_7_plot, "%d %lf %lf\n", year - 1800, yearly_land_avgs[i],
              yearly_land_avgs[i + 100]);
    }
  }
}

void solve_problem_8(FILE *problem_8_plot,
                     double yearly_avgs[DATA_COLS][MAX_ROWS],
                     int yearly_avgs_date[DATA_COLS][MAX_ROWS][3],
                     int yearly_avgs_length[DATA_COLS]) {
  int cols[] = {0, 2, 4};

  // Print header
  fprintf(
      problem_8_plot,
      "# Year LandAverageTemperature LandMaxTemperature LandMinTemperature\n");

  // For every row
  for (int i = 0; i < yearly_avgs_length[0]; i++) {
    int year = yearly_avgs_date[0][i][0];

    if (year >= 1850 && year <= 2015) {
      fprintf(problem_8_plot, "%d ", year);

      // For every column
      for (int j = 0; j < 3; j++) {
        fprintf(problem_8_plot, "%lf ", yearly_avgs[cols[j]][i]);
      }

      fprintf(problem_8_plot, "\n");
    }
  }
}

void solve_problem_9(FILE *problem_9_plot,
                     double data_temps[DATA_COLS][MAX_ROWS],
                     int data_dates[MAX_ROWS][3], int data_length) {
  double century_avgs[3][3],    // [century][data column]
      century_sums[3][3] = {0}; // [century][data column]
  int century_avgs_length[3][3] = {0}, cols[3] = {0, 2, 4},
      centuries_start[3] = {1850, 1900, 2000},
      centuries_end[3] = {1899, 1999, 2015};

  // 1. Gather data
  // For every column of data
  for (int c = 0; c < 3; c++) {
    // For every row of that column of data
    for (int r = 0; r < data_length; r++) {
      int year = data_dates[r][0], month = data_dates[r][1];

      // For every century
      for (int i = 0; i < 3; i++) {
        if (year >= centuries_start[i] && year <= centuries_end[i]) {
          // Add to total century's sum and length of years
          century_sums[i][c] += data_temps[cols[c]][r];
          century_avgs_length[i][c] += 1;

          // If on the last month of the last year of the century, calculate
          // century's average
          if (year == centuries_end[i] && month == 12) {
            century_avgs[i][c] = century_sums[i][c] / century_avgs_length[i][c];
          }
        }
      }
    }
  }

  // 2. Print to file
  // Print header
  fprintf(
      problem_9_plot,
      "# Years LandAverageTemperature LandMaxTemperature LandMinTemperature\n");

  // For every century
  for (int c = 0; c < 3; c++) {
    // Print date
    fprintf(problem_9_plot, "%d-%d ", centuries_start[c], centuries_end[c]);

    // Print values
    fprintf(problem_9_plot, "%lf %lf %lf", century_avgs[c][0],
            century_avgs[c][1], century_avgs[c][2]);

    // Next line
    fprintf(problem_9_plot, "\n");
  }
}

void solve_problem_10(FILE *problem_10_plot,
                      double data_temps[DATA_COLS][MAX_ROWS],
                      int data_dates[MAX_ROWS][3], int data_length) {
  double monthly_uncerts_sum[MONTHS_COUNT] = {0},
         monthly_uncerts_avg[MONTHS_COUNT];
  int monthly_uncerts_length[MONTHS_COUNT] = {0};

  double monthly_temps_sum[MONTHS_COUNT] = {0}, monthly_temps_avg[MONTHS_COUNT];
  int monthly_temps_length[MONTHS_COUNT] = {0};

  // For every year
  for (int i = 0; i < data_length; i++) {
    int year = data_dates[i][0], month = data_dates[i][1];

    if (year >= 2000 && year <= 2015) {
      // Add to sum and lengths for current month
      monthly_uncerts_sum[month - 1] += data_temps[1][i];
      monthly_uncerts_length[month - 1] += 1;
      monthly_temps_sum[month - 1] += data_temps[0][i];
      monthly_temps_length[month - 1] += 1;

      // If last year, calculate averages
      if (year == 2015) {
        monthly_uncerts_avg[month - 1] =
            monthly_uncerts_sum[month - 1] / monthly_uncerts_length[month - 1];
        monthly_temps_avg[month - 1] =
            monthly_temps_sum[month - 1] / monthly_temps_length[month - 1];
      }
    }
  }

  // Print header
  fprintf(problem_10_plot,
          "# Month AverageLandTemp AverageLandTempUncertainty\n");

  // Print lines
  for (int i = 0; i < MONTHS_COUNT; i++) {
    fprintf(problem_10_plot, "%d %lf %lf\n", i + 1, monthly_temps_avg[i],
            monthly_uncerts_avg[i]);
  }
}

void solve_problem_11(FILE *problem_11_plot,
                      double yearly_avgs[DATA_COLS][MAX_ROWS],
                      int yearly_avgs_date[DATA_COLS][MAX_ROWS][3],
                      int yearly_avgs_length[DATA_COLS]) {
  int cols[2] = {0, 6};

  // Print header
  fprintf(problem_11_plot,
          "# Year LandAverageTemperature LandAndOceanAverageTemperature\n");

  // Print lines
  for (int i = 0; i < yearly_avgs_length[0]; i++) {
    int year = yearly_avgs_date[0][i][0];

    if (year >= 1850 && year <= 2015) {

      // Print year
      fprintf(problem_11_plot, "%d ", year);

      for (int c = 0; c < 2; c++) {
        fprintf(problem_11_plot, "%lf ", yearly_avgs[cols[c]][i]);
      }

      fprintf(problem_11_plot, "\n");
    }
  }
}

int main(void) {
  // Declare variables
  FILE *csv_file = fopen("GlobalTemperatures.csv", "r");
  double data_temps[DATA_COLS][MAX_ROWS];
  int data_dates[MAX_ROWS][3], data_length = 0;

  // Read data
  read_data(csv_file, data_dates, data_temps, &data_length);

  // Print data (for development)
  // print_data(data_dates, data_temps, data_length);

  // PROBLEM #1
  double yearly_land_avgs[MAX_ROWS];
  int yearly_land_avgs_date[MAX_ROWS][3], yearly_land_avgs_length = 0;
  solve_problem_1(data_dates, data_temps[0], data_length, yearly_land_avgs_date,
                  yearly_land_avgs, &yearly_land_avgs_length);

  // PROBLEM #2
  double century_land_avgs[CENTURIES_COUNT];
  int century_land_avgs_length[CENTURIES_COUNT] = {0};
  solve_problem_2(data_dates, data_temps[0], data_length, century_land_avgs,
                  century_land_avgs_length);

  // PROBLEM #3
  double monthly_avgs[MONTHS_COUNT];
  int monthly_avgs_length[MONTHS_COUNT] = {0};
  solve_problem_3(data_dates, data_temps[0], data_length, monthly_avgs,
                  monthly_avgs_length);

  // PROBLEM #4
  int hottest_temp_date[3], coldest_temp_date[3];
  double hottest_temp, coldest_temp;
  solve_problem_4(data_dates, data_temps[0], data_length, hottest_temp_date,
                  coldest_temp_date, &hottest_temp, &coldest_temp);

  // PROBLEM #5
  int hottest_yearly_avg_date[3], coldest_yearly_avg_date[3];
  double hottest_yearly_avg, coldest_yearly_avg;
  solve_problem_5(yearly_land_avgs, yearly_land_avgs_date,
                  yearly_land_avgs_length, hottest_yearly_avg_date,
                  coldest_yearly_avg_date, &hottest_yearly_avg,
                  &coldest_yearly_avg);

  // PROBLEM #6
  FILE *problem_6_plot = fopen("problem_6_plot.txt", "w");
  solve_problem_6(problem_6_plot, yearly_land_avgs, yearly_land_avgs_date,
                  yearly_land_avgs_length);
  fclose(problem_6_plot);

  // PROBLEM #7
  FILE *problem_7_plot = fopen("problem_7_plot.txt", "w");
  solve_problem_7(problem_7_plot, yearly_land_avgs, yearly_land_avgs_date,
                  yearly_land_avgs_length);
  fclose(problem_7_plot);

  // Calculate yearly averages for problems 8 and 11
  double yearly_avgs[DATA_COLS][MAX_ROWS];
  int yearly_avgs_date[DATA_COLS][MAX_ROWS][3],
      yearly_avgs_length[DATA_COLS] = {0};
  calc_yearly_avgs(data_temps, data_dates, data_length, yearly_avgs,
                   yearly_avgs_date, yearly_avgs_length);

  // PROBLEM #8
  FILE *problem_8_plot = fopen("problem_8_plot.txt", "w");
  int yearly_temps_avgs_length = 0, yearly_temps_avgs_date[3][MAX_ROWS];
  double yearly_temps_avgs[3][MAX_ROWS];
  solve_problem_8(problem_8_plot, yearly_avgs, yearly_avgs_date,
                  yearly_avgs_length);
  fclose(problem_8_plot);

  // PROBLEM #9
  FILE *problem_9_plot = fopen("problem_9_plot.txt", "w");
  solve_problem_9(problem_9_plot, data_temps, data_dates, data_length);
  fclose(problem_9_plot);

  // PROBLEM #10
  FILE *problem_10_plot = fopen("problem_10_plot.txt", "w");
  solve_problem_10(problem_10_plot, data_temps, data_dates, data_length);
  fclose(problem_10_plot);

  // PROBLEM #11
  FILE *problem_11_plot = fopen("problem_11_plot.txt", "w");
  solve_problem_11(problem_11_plot, yearly_avgs, yearly_avgs_date,
                   yearly_avgs_length);
  fclose(problem_11_plot);

  return 0;
}
