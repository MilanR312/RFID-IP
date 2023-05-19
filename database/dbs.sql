--
-- PostgreSQL database dump
--

-- Dumped from database version 15.2
-- Dumped by pg_dump version 15.2

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
-- Name: artikel; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.artikel (
    code character varying NOT NULL,
    naam character varying,
    beschikbaar integer DEFAULT 0,
    prijs integer,
    soort character varying
);


ALTER TABLE public.artikel OWNER TO postgres;

--
-- Name: gebruiker; Type: TABLE; Schema: public; Owner: postgres
--

CREATE TABLE public.gebruiker (
    id character varying NOT NULL,
    naam character varying NOT NULL,
    leeftijd integer NOT NULL,
    adres character varying NOT NULL,
    telefoonnummer character varying NOT NULL,
    functie character varying NOT NULL
);


ALTER TABLE public.gebruiker OWNER TO postgres;

--
-- Name: gebruiker_id_seq; Type: SEQUENCE; Schema: public; Owner: postgres
--

CREATE SEQUENCE public.gebruiker_id_seq
    AS integer
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;


ALTER TABLE public.gebruiker_id_seq OWNER TO postgres;

--
-- Name: gebruiker_id_seq; Type: SEQUENCE OWNED BY; Schema: public; Owner: postgres
--

ALTER SEQUENCE public.gebruiker_id_seq OWNED BY public.gebruiker.id;


--
-- Name: gebruiker id; Type: DEFAULT; Schema: public; Owner: postgres
--

ALTER TABLE ONLY public.gebruiker ALTER COLUMN id SET DEFAULT nextval('public.gebruiker_id_seq'::regclass);


--
-- Data for Name: artikel; Type: TABLE DATA; Schema: public; Owner: postgres
--


--
-- Data for Name: gebruiker; Type: TABLE DATA; Schema: public; Owner: postgres
--


--
-- Name: gebruiker_id_seq; Type: SEQUENCE SET; Schema: public; Owner: postgres
--

SELECT pg_catalog.setval('public.gebruiker_id_seq', 4, true);


--
-- Name: TABLE artikel; Type: ACL; Schema: public; Owner: postgres
--

GRANT ALL ON TABLE public.artikel TO "ESP32";


--
-- Name: TABLE gebruiker; Type: ACL; Schema: public; Owner: postgres
--

GRANT ALL ON TABLE public.gebruiker TO "ESP32";


--
-- PostgreSQL database dump complete
--

