
CREATE TABLE IF NOT EXISTS landmark (
    id INTEGER PRIMARY KEY,
    name TEXT,
    description TEXT,
    icon_url TEXT,
    latitude REAL,
    longitude REAL,
    altitude REAL,
    radius REAL,
    top_left_lat REAL,
    top_left_lon REAL,
    bottom_right_lat REAL,
    bottom_right_lon REAL,
    country TEXT,
    country_code TEXT,
    state TEXT,
    county TEXT,
    district TEXT,
    city TEXT,
    thoroughfare_name TEXT,
    thoroughfare_number TEXT,
    postcode TEXT,
    post_office_box TEXT,
    phone TEXT,
    url TEXT
);
@@@
CREATE TABLE IF NOT EXISTS landmark_attribute (
    landmark_id INTEGER,
    key TEXT,
    value TEXT,
    PRIMARY KEY (landmark_id,key)
);
@@@
CREATE INDEX IF NOT EXISTS landmark_attribute_fk_index ON landmark_attribute(landmark_id);

CREATE TABLE IF NOT EXISTS category (
    id INTEGER PRIMARY KEY,
    name TEXT,
    description TEXT,
    icon_url TEXT
);
@@@
CREATE TABLE IF NOT EXISTS category_attribute (
    category_id INTEGER,
    key TEXT,
    value TEXT,
    PRIMARY KEY (category_id,key)
);
@@@
CREATE INDEX IF NOT EXISTS category_attribute_fk_index ON category_attribute(category_id);

CREATE TABLE IF NOT EXISTS landmark_category (
    landmark_id INTEGER,
    category_id INTEGER,
    PRIMARY KEY (landmark_id,category_id)
);
@@@
CREATE TRIGGER IF NOT EXISTS add_landmark_notification
AFTER INSERT ON landmark
FOR EACH ROW
    BEGIN
        DELETE FROM landmark_notification
        WHERE timestamp < strftime('%s', 'now', '-2 minutes');
        INSERT INTO landmark_notification(timestamp, action, landmark_id)
        VALUES(strftime("%f", "now") - strftime("%S", "now") + strftime("%s"),'CHANGE', new.id);
    END;
@@@
CREATE TRIGGER IF NOT EXISTS remove_landmark_notification
BEFORE DELETE ON landmark
FOR EACH ROW
    BEGIN
        DELETE FROM landmark_notification
        WHERE timestamp < strftime('%s', 'now', '-2 minutes');
        INSERT INTO landmark_notification(timestamp, action, landmark_id)
        VALUES(strftime("%f", "now") - strftime("%S", "now") + strftime("%s"),'REMOVE', old.id);
    END;
@@@
CREATE TRIGGER IF NOT EXISTS add_category_notification
AFTER INSERT ON category
FOR EACH ROW
    BEGIN
        DELETE FROM category_notification
        WHERE timestamp < strftime('%s', 'now', '-2 minutes');
        INSERT INTO category_notification(timestamp, action, category_id)
        VALUES(strftime("%f", "now") - strftime("%S", "now") + strftime("%s"),'ADD', new.id);
    END;
@@@
CREATE TRIGGER IF NOT EXISTS change_category_notification
AFTER UPDATE ON category
FOR EACH ROW
    BEGIN
        DELETE FROM category_notification
        WHERE timestamp < strftime('%s', 'now', '-2 minutes');
        INSERT INTO category_notification(timestamp, action, category_id)
        VALUES(strftime("%f", "now") - strftime("%S", "now") + strftime("%s"),'CHANGE', new.id);
    END;
@@@
CREATE TRIGGER IF NOT EXISTS remove_category_notification
BEFORE DELETE ON category
FOR EACH ROW
    BEGIN
        DELETE FROM category_notification
        WHERE timestamp < strftime('%s', 'now', '-2 minutes');
        INSERT INTO category_notification(timestamp, action, category_id)
        VALUES(strftime("%f", "now") - strftime("%S", "now") + strftime("%s"),'REMOVE', old.id);
    END;
@@@
CREATE INDEX IF NOT EXISTS landmark_category_fk_lm_index ON landmark_category(landmark_id);
@@@
CREATE INDEX IF NOT EXISTS landmark_category_fk_cat_index ON landmark_category(category_id);

