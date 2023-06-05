#include "device_controller.h"
#define _CRT_SECURE_NO_WARNINGS

void device_controller::set_borders() {
	result_t result = get_edges_settings_calb(device, &edges_settings_calb, &calibration);
	if (result != result_ok) {
        throw device_error("Не получилось получить координаты границ");
	}

	edges_settings_calb.LeftBorder = 0.;
    edges_settings_calb.RightBorder = 550.;
	result = set_edges_settings_calb(device, &edges_settings_calb, &calibration);

	if (result != result_ok) {
        throw device_error("Не получилось установить координаты границ");
	}
}

void device_controller::wait_for_stop(int update_interval) {
	result_t result = command_wait_for_stop(device, update_interval);
	if (result != result_ok) {
        throw device_error("Не получилось выполнить команду ожидания остановки");
	}
}

void device_controller::update_status() {
	result_t result = get_status_calb(device, &status_calb, &calibration);
	if (result != result_ok) {
        throw device_error("Не получилось получить текущее состояние");
	}
}

void device_controller::connect_controller() {
	device_enumeration_t devenum = enumerate_devices(ENUMERATE_PROBE, NULL);
	if (!devenum) {
        throw device_error("Не получилось считать все доступные устройства");
	}

	if (get_device_count(devenum) == 0) {
        throw device_error("Найдено 0 устройств");
	}

	char device_name[256];
	char* res = strcpy(device_name, get_device_name(devenum, 0));
	if (!res) {
        throw device_error("Не получилось считать имя устройства");
	}
	free_enumerate_devices(devenum);

	device = open_device(device_name);
	if (device == device_undefined) {
        throw device_error("Не получилось открыть устройство");
	}
    this->is_connected = true;

	calibration.A = 1. / 80.;
	calibration.MicrostepMode = MICROSTEP_MODE_FRAC_256;
	this->set_borders();
}

void device_controller::disconnect_controller() {
	result_t result = close_device(&device);
    if (this->is_connected) {
        if (result != result_ok) {
            throw device_error("Не получилось отключиться от устройства");
        }
    }
}

void device_controller::move_right_msec(int msec) {
    if (msec < 0) {
        throw device_error("Got incorrect time in command move_right_msec");
    }

    result_t result = command_right(device);
    if (result != result_ok) {
        throw device_error("Error command right");
    }

    msec_sleep(msec);
}

void device_controller::move_left_msec(int msec) {
    if (msec < 0) {
        throw device_error("Got incorrect time in command move_left_msec");
    }

    result_t result = command_left(device);
    if (result != result_ok) {
        throw device_error("Error command left");
    }
    msec_sleep(msec);
}

void device_controller::stop() {
    result_t result = command_stop(device);
    if (result != result_ok) {
        throw device_error("Не получилось выполнить остановку");
    }
}

void device_controller::move_to(float x) {
	result_t result;
    this->wait_for_stop();
    this->update_status();

	if (x > edges_settings_calb.LeftBorder && x < edges_settings_calb.RightBorder) {
        if (this->table_applied) {
            int l = floor(x);
            int r = ceil(x);
            if (l == r && l != 0 && r != 540) {
                x += this->table[l-1];
            }
            if (l != r && l != 0 && r != 540) {
                x += this->table[r-1] * (x - l) + this->table[l-1] * (r - x);
            }
        }
		result = command_move_calb(device, x, &calibration);
		if (result != result_ok) {
            throw device_error("Не получилось выполнить команду абсолютное перемещение");
		}
		return;
	}
	
	if (x < edges_settings_calb.LeftBorder) {
		result = command_move_calb(device, edges_settings_calb.LeftBorder, &calibration);
		if (result != result_ok) {
            throw device_error("Не получилось выполнить команду абсолютное перемещение");
		}
		return;
	}

	if (x > edges_settings_calb.RightBorder) {
        result = command_move_calb(device, edges_settings_calb.RightBorder, &calibration);
		if (result != result_ok) {
            throw device_error("Не получилось выполнить команду абсолютное перемещение");
		}
		return;
	}
}

void device_controller::move(float delta_x) {
	result_t result;
    this->wait_for_stop();
    this->update_status();
	float target_position = status_calb.CurPosition + delta_x;

    if (this->table_applied) {
        int l = floor(target_position);
        int r = ceil(target_position);
        if (l == r && l != 0 && r != 540) {
            target_position += this->table[l-1];
        }
        if (l != r && l != 0 && r != 540) {
            target_position += this->table[r-1] * (target_position - l) + this->table[l-1] * (r - target_position);
        }
    }

	if (target_position < edges_settings_calb.RightBorder && target_position > edges_settings_calb.LeftBorder) {
		result = command_movr_calb(device, delta_x, &calibration);
		if (result != result_ok) {
            throw device_error("Не получилось выполнить команду относительное смещение");
		}
		return;
	}

	if (target_position > edges_settings_calb.RightBorder) {
		this->move_to(edges_settings_calb.RightBorder);
		return;
	}

	if (target_position < edges_settings_calb.LeftBorder) {
		this->move_to(edges_settings_calb.LeftBorder);
		return;
	}
}

void device_controller::home() {
	result_t result = command_homezero(device);
    this->wait_for_stop();
    this->set_borders();
	if (result != result_ok) {
        throw device_error("Не получилось выполнить команду домой");
	}
}

std::string device_controller::get_position(bool wait) {
    if (wait) {
        this->wait_for_stop();
    }

    this->update_status();
    return std::to_string(status_calb.CurPosition);
}

bool device_controller::get_connection_status() {
    return this->is_connected;
}

void device_controller::read_file(const QString &filename)
{
    if (filename.isEmpty()) {
        return;
    }
    FILE* f = fopen(filename.toStdString().c_str(), "rb");
    if (!f) {
        throw device_error("Не получилось открыть файл с таблицей корректировок");
    }
    fseek(f, 0, SEEK_END);
    int file_size = ftell(f);
    if (file_size != this->table_size) {
        fclose(f);
        throw device_error("Получен файл неверного размера");
    }
    fseek(f, 0, SEEK_SET);
    this->table = (float*)malloc(this->table_size);
    if (!this->table) {
        fclose(f);
        throw device_error("Не получилось выделить память под таблицу корректировок");
    }
    int read = fread(this->table, sizeof(float), this->table_size, f);
    fclose(f);
    if (read != TABLE_ELEMENTS_COUNT) {
        throw device_error("Считано не верное количество чисел из таблицы корректировок. Файл неисправен");
    }
    this->table_applied = true;
}

void device_controller::create_file(const QString &filename, float *new_arr) {
    if (filename.isEmpty()) {
        return;
    }
    FILE *f = fopen(filename.toStdString().c_str(), "wb");
    if (!f) {
        fclose(f);
        throw device_error("Не получилось открыть файл с таблицей корректировок для записи");
    }
    int size = fwrite(new_arr, sizeof(float), this->table_size, f);
    fclose(f);
    if (size != this->table_size) {
        throw device_error("Записалиь не все элементы в таблицу корректировок");
    }
}


device_controller::device_controller() {
    is_connected = false;
    table = NULL;
    table_applied = false;
    table_size = TABLE_ELEMENTS_COUNT * sizeof(float);
}

device_controller::~device_controller() {
    free(this->table);
}
