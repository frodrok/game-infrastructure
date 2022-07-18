--
-- PostgreSQL database dump
--

-- Dumped from database version 12.10 (Debian 12.10-1.pgdg110+1)
-- Dumped by pg_dump version 12.11 (Ubuntu 12.11-0ubuntu0.20.04.1)

SET statement_timeout = 0;
SET lock_timeout = 0;
SET idle_in_transaction_session_timeout = 0;
SET client_encoding = 'UTF8';
SET standard_conforming_strings = on;
SELECT pg_catalog.set_config('search_path', '', false);
SET check_function_bodies = false;
SET xmloption = content;
SET client_min_messages = warning;
SET row_security = off;

--
-- Name: uuid-ossp; Type: EXTENSION; Schema: -; Owner: -
--

CREATE EXTENSION IF NOT EXISTS "uuid-ossp" WITH SCHEMA public;


--
-- Name: EXTENSION "uuid-ossp"; Type: COMMENT; Schema: -; Owner: 
--

COMMENT ON EXTENSION "uuid-ossp" IS 'generate universally unique identifiers (UUIDs)';


SET default_tablespace = '';

SET default_table_access_method = heap;

--
-- Name: account; Type: TABLE; Schema: public; Owner: gameservice
--

CREATE TABLE public.account (
    id uuid DEFAULT public.uuid_generate_v4() NOT NULL,
    username character varying(255),
    password character varying(255),
    access_token character varying(255)
);


ALTER TABLE public.account OWNER TO gameservice;

--
-- Name: character; Type: TABLE; Schema: public; Owner: gameservice
--

CREATE TABLE public."character" (
    id uuid DEFAULT public.uuid_generate_v4() NOT NULL,
    name character varying(255),
    level integer,
    experience integer,
    class character varying(255),
    race character varying(255),
    account_id uuid
);


ALTER TABLE public."character" OWNER TO gameservice;

--
-- Data for Name: account; Type: TABLE DATA; Schema: public; Owner: gameservice
--

COPY public.account (id, username, password, access_token) FROM stdin;
371ff048-07e8-4e9b-8e40-d0f2ef4f1561	mujamba	14c6bbde9e21961a31302e2dff33d05802e6109b54e40b9f8542225607c3685a7d078dba259edbab291b6d1999e34c162d657f30e26d6cf52ce73e8a95296bcd	99279ebdd0cb3324d3d91584c59696f327a6794c83f6db9001d72f4491fe902588f322bcd77add293e7d06c76f831a587aeb1488b14cc382410e916ba4f40641
df5fe88c-bcb4-4c43-836d-af8c0125f10d	fredrik	hollinger	2999ca0f0ec7eea83e3e0ca19f8ff5efb9753199892761535f3bd06ece4cccf315e99c8dc2a37e0e06ca678bdfae3ba360ecf7b69f63420ed61f76f3c635e648
\.


--
-- Data for Name: character; Type: TABLE DATA; Schema: public; Owner: gameservice
--

COPY public."character" (id, name, level, experience, class, race, account_id) FROM stdin;
183ff3e6-793b-4738-ab93-4e4ec4209adf	Everhard	70	656600	Mage	Undead	371ff048-07e8-4e9b-8e40-d0f2ef4f1561
\.


--
-- Name: account account_pkey; Type: CONSTRAINT; Schema: public; Owner: gameservice
--

ALTER TABLE ONLY public.account
    ADD CONSTRAINT account_pkey PRIMARY KEY (id);


--
-- Name: character character_id_key; Type: CONSTRAINT; Schema: public; Owner: gameservice
--

ALTER TABLE ONLY public."character"
    ADD CONSTRAINT character_id_key UNIQUE (id);


--
-- Name: character character_pkey; Type: CONSTRAINT; Schema: public; Owner: gameservice
--

ALTER TABLE ONLY public."character"
    ADD CONSTRAINT character_pkey PRIMARY KEY (id);


--
-- Name: character id_unique; Type: CONSTRAINT; Schema: public; Owner: gameservice
--

ALTER TABLE ONLY public."character"
    ADD CONSTRAINT id_unique UNIQUE (id);


--
-- Name: account uniq_id; Type: CONSTRAINT; Schema: public; Owner: gameservice
--

ALTER TABLE ONLY public.account
    ADD CONSTRAINT uniq_id UNIQUE (id);


--
-- Name: account unique_account_id; Type: CONSTRAINT; Schema: public; Owner: gameservice
--

ALTER TABLE ONLY public.account
    ADD CONSTRAINT unique_account_id UNIQUE (id);


--
-- Name: character unique_character_id; Type: CONSTRAINT; Schema: public; Owner: gameservice
--

ALTER TABLE ONLY public."character"
    ADD CONSTRAINT unique_character_id UNIQUE (id);


--
-- Name: account unique_username; Type: CONSTRAINT; Schema: public; Owner: gameservice
--

ALTER TABLE ONLY public.account
    ADD CONSTRAINT unique_username UNIQUE (username);


--
-- Name: character account; Type: FK CONSTRAINT; Schema: public; Owner: gameservice
--

ALTER TABLE ONLY public."character"
    ADD CONSTRAINT account FOREIGN KEY (account_id) REFERENCES public.account(id);


--
-- PostgreSQL database dump complete
--

