CREATE
EXTENSION IF NOT EXISTS "uuid-ossp";

CREATE TABLE IF NOT EXISTS English
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
    Length INTEGER,
    Object JSONB,
    List INTEGER []
    );