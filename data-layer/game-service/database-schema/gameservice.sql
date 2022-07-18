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

SET default_tablespace = '';

SET default_table_access_method = heap;

--
-- Name: account; Type: TABLE; Schema: public; Owner: gameservice
--

CREATE TABLE public.account (
    id uuid,
    username character varying(255),
    password character varying(255)
);


ALTER TABLE public.account OWNER TO gameservice;

--
-- Name: character; Type: TABLE; Schema: public; Owner: gameservice
--

CREATE TABLE public."character" (
    id uuid,
    name character varying(255),
    level integer,
    experience integer,
    class character varying(255),
    race character varying(255),
    account_id uuid
);


ALTER TABLE public."character" OWNER TO gameservice;

--
-- Name: character character_id_key; Type: CONSTRAINT; Schema: public; Owner: gameservice
--

ALTER TABLE ONLY public."character"
    ADD CONSTRAINT character_id_key UNIQUE (id);


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
-- Name: character account; Type: FK CONSTRAINT; Schema: public; Owner: gameservice
--

ALTER TABLE ONLY public."character"
    ADD CONSTRAINT account FOREIGN KEY (account_id) REFERENCES public.account(id);


--
-- PostgreSQL database dump complete
--

