CREATE
EXTENSION IF NOT EXISTS "uuid-ossp";

CREATE TABLE IF NOT EXISTS Italian
(
    Id
    UUID
    PRIMARY
    KEY
    DEFAULT
    uuid_generate_v4
(
),
    Greet VARCHAR
(
    45
) NOT NULL,
    Kind BOOLEAN NOT NULL,
    Length INTEGER
    );