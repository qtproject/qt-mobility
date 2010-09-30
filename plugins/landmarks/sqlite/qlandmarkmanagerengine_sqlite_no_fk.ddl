CREATE TABLE IF NOT EXISTS landmark (
    id INTEGER PRIMARY KEY,
    name TEXT,
    latitude REAL,
    longitude REAL,
    altitude REAL
);
@@@ 
CREATE TABLE IF NOT EXISTS landmark_attribute(
    landmarkID INTEGER,
    key TEXT,
    value BLOB,
    PRIMARY KEY(landmarkId,key)
);
@@@
CREATE TABLE IF NOT EXISTS category (
    id INTEGER PRIMARY KEY,
    name TEXT
);
@@@
CREATE TABLE IF NOT EXISTS category_attribute(
    categoryId INTEGER,
    key TEXT,
    value BLOB,
    PRIMARY KEY (categoryId,key)
);
@@@
CREATE TABLE IF NOT EXISTS landmark_category (
    landmarkId INTEGER,
    categoryId INTEGER,
    PRIMARY KEY (landmarkId,categoryId)
);
@@@
CREATE TABLE IF NOT EXISTS landmark_notification (
    timestamp INTEGER,
    action TEXT,
    landmarkId INTEGER,
    PRIMARY KEY(timestamp, action, landmarkId)
);
@@@
CREATE TABLE IF NOT EXISTS category_notification (
    timestamp INTEGER,
    action TEXT,
    categoryId INTEGER,
    PRIMARY KEY(timestamp, action, categoryId)
);
@@@
CREATE TRIGGER IF NOT EXISTS add_landmark_notification
AFTER INSERT ON landmark
FOR EACH ROW
    BEGIN
        DELETE FROM landmark_notification
        WHERE timestamp < strftime('%s', 'now', '-2 minutes');
        INSERT INTO landmark_notification(timestamp, action, landmarkId)
        VALUES((strftime("%f", "now") - strftime("%S", "now") + strftime("%s"))*1000,'ADD', new.id);
    END;
@@@
CREATE TRIGGER IF NOT EXISTS change_landmark_notification
AFTER UPDATE ON landmark
FOR EACH ROW
    BEGIN
        DELETE FROM landmark_notification
        WHERE timestamp < strftime('%s', 'now', '-2 minutes');
        INSERT INTO landmark_notification(timestamp, action, landmarkId)
        VALUES((strftime("%f", "now") - strftime("%S", "now") + strftime("%s"))*1000,'CHANGE', new.id);
    END;
@@@
CREATE TRIGGER IF NOT EXISTS remove_landmark_notification
BEFORE DELETE ON landmark
FOR EACH ROW
    BEGIN
        DELETE FROM landmark_notification
        WHERE timestamp < strftime('%s', 'now', '-2 minutes');
        INSERT INTO landmark_notification(timestamp, action, landmarkId)
        VALUES((strftime("%f", "now") - strftime("%S", "now") + strftime("%s"))*1000,'REMOVE', old.id);
    END;
@@@
CREATE TRIGGER IF NOT EXISTS add_category_notification
AFTER INSERT ON category
FOR EACH ROW
    BEGIN
        DELETE FROM category_notification
        WHERE timestamp < strftime('%s', 'now', '-2 minutes');
        INSERT INTO category_notification(timestamp, action, categoryId)
        VALUES((strftime("%f", "now") - strftime("%S", "now") + strftime("%s"))*1000,'ADD', new.id);
    END;
@@@
CREATE TRIGGER IF NOT EXISTS change_category_notification
AFTER UPDATE ON category
FOR EACH ROW
    BEGIN
        DELETE FROM category_notification
        WHERE timestamp < strftime('%s', 'now', '-2 minutes');
        INSERT INTO category_notification(timestamp, action, categoryId)
        VALUES((strftime("%f", "now") - strftime("%S", "now") + strftime("%s"))*1000,'CHANGE', new.id);
    END;
@@@
CREATE TRIGGER IF NOT EXISTS remove_category_notification
BEFORE DELETE ON category
FOR EACH ROW
    BEGIN
        DELETE FROM category_notification
        WHERE timestamp < strftime('%s', 'now', '-2 minutes');
        INSERT INTO category_notification(timestamp, action, categoryId)
        VALUES((strftime("%f", "now") - strftime("%S", "now") + strftime("%s"))*1000,'REMOVE', old.id);
    END;
@@@
CREATE INDEX IF NOT EXISTS landmark_category_fk_lm_index ON landmark_category(landmarkId);
@@@
CREATE INDEX IF NOT EXISTS landmark_category_fk_cat_index ON landmark_category(categoryId);
