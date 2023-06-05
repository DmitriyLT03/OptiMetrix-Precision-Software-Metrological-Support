#pragma once

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <wchar.h>
#include <locale.h>
#include <ximc.h>
#include <io.h>
#include <fcntl.h>
#include <string>
#include <QString>
#include <QFile>
#include <QDebug>
#include <device_error.h>
#include <math.h>

#define TABLE_ELEMENTS_COUNT 540

class device_controller {
private:
    bool is_connected = false;
	device_t device;
	calibration_t calibration;
	edges_settings_calb_t edges_settings_calb;
	status_calb_t status_calb;

	void set_borders();
	void wait_for_stop(int update_interval = 10);
	void update_status();
    int table_size;
public:
    bool table_applied;
    float* table;
    device_controller();
    ~device_controller();
	void connect_controller();
	void disconnect_controller();

	void move_to(float x);
	void move(float delta_x);
	void home();

    void move_right_msec(int msec);
    void move_left_msec(int msec);
    void stop();

	void print_position(bool wait = false);
	float get_speed(bool wait = false);
    std::string get_position(bool wait = false);

    bool get_connection_status();

    void read_file(const QString &filename);
    void create_file(const QString &filename, float *new_arr);
};
