/*
 * cpcraft-port — engine/math_lut.c
 *
 * Pre-computed lookup tables.
 *
 * The 8-bit sin LUT is generated at build time via a constexpr table in
 * C++ — but we want this to compile as C too (so the engine links into
 * either language), so we generate the table here with a static initializer
 * using a small macro.
 *
 * The 16.16 fixed-point sintable_deg10 is borrowed verbatim from
 * CP-Raycaster-Demo/src/math_tables.h (diddyholz) so we can reuse the
 * same raycaster math if we ever want a CP-Raycaster-style renderer.
 */
#include "math_lut.h"
#include <math.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

/* Build sin_lut8 with a file-scope initializer.
 *
 * C doesn't allow constexpr arrays, but it DOES allow static initializers
 * with computed float expressions. The compiler evaluates this at build
 * time, so there's no runtime cost — the .rodata section gets the table
 * pre-baked. */
#define SIN8(i) ((uint8_t)(sinf((i) * (2.0f * (float)M_PI) / 256.0f) * 127.5f + 127.5f))
const uint8_t sin_lut8[256] = {
    SIN8(0),   SIN8(1),   SIN8(2),   SIN8(3),   SIN8(4),   SIN8(5),   SIN8(6),   SIN8(7),
    SIN8(8),   SIN8(9),   SIN8(10),  SIN8(11),  SIN8(12),  SIN8(13),  SIN8(14),  SIN8(15),
    SIN8(16),  SIN8(17),  SIN8(18),  SIN8(19),  SIN8(20),  SIN8(21),  SIN8(22),  SIN8(23),
    SIN8(24),  SIN8(25),  SIN8(26),  SIN8(27),  SIN8(28),  SIN8(29),  SIN8(30),  SIN8(31),
    SIN8(32),  SIN8(33),  SIN8(34),  SIN8(35),  SIN8(36),  SIN8(37),  SIN8(38),  SIN8(39),
    SIN8(40),  SIN8(41),  SIN8(42),  SIN8(43),  SIN8(44),  SIN8(45),  SIN8(46),  SIN8(47),
    SIN8(48),  SIN8(49),  SIN8(50),  SIN8(51),  SIN8(52),  SIN8(53),  SIN8(54),  SIN8(55),
    SIN8(56),  SIN8(57),  SIN8(58),  SIN8(59),  SIN8(60),  SIN8(61),  SIN8(62),  SIN8(63),
    SIN8(64),  SIN8(65),  SIN8(66),  SIN8(67),  SIN8(68),  SIN8(69),  SIN8(70),  SIN8(71),
    SIN8(72),  SIN8(73),  SIN8(74),  SIN8(75),  SIN8(76),  SIN8(77),  SIN8(78),  SIN8(79),
    SIN8(80),  SIN8(81),  SIN8(82),  SIN8(83),  SIN8(84),  SIN8(85),  SIN8(86),  SIN8(87),
    SIN8(88),  SIN8(89),  SIN8(90),  SIN8(91),  SIN8(92),  SIN8(93),  SIN8(94),  SIN8(95),
    SIN8(96),  SIN8(97),  SIN8(98),  SIN8(99),  SIN8(100), SIN8(101), SIN8(102), SIN8(103),
    SIN8(104), SIN8(105), SIN8(106), SIN8(107), SIN8(108), SIN8(109), SIN8(110), SIN8(111),
    SIN8(112), SIN8(113), SIN8(114), SIN8(115), SIN8(116), SIN8(117), SIN8(118), SIN8(119),
    SIN8(120), SIN8(121), SIN8(122), SIN8(123), SIN8(124), SIN8(125), SIN8(126), SIN8(127),
    SIN8(128), SIN8(129), SIN8(130), SIN8(131), SIN8(132), SIN8(133), SIN8(134), SIN8(135),
    SIN8(136), SIN8(137), SIN8(138), SIN8(139), SIN8(140), SIN8(141), SIN8(142), SIN8(143),
    SIN8(144), SIN8(145), SIN8(146), SIN8(147), SIN8(148), SIN8(149), SIN8(150), SIN8(151),
    SIN8(152), SIN8(153), SIN8(154), SIN8(155), SIN8(156), SIN8(157), SIN8(158), SIN8(159),
    SIN8(160), SIN8(161), SIN8(162), SIN8(163), SIN8(164), SIN8(165), SIN8(166), SIN8(167),
    SIN8(168), SIN8(169), SIN8(170), SIN8(171), SIN8(172), SIN8(173), SIN8(174), SIN8(175),
    SIN8(176), SIN8(177), SIN8(178), SIN8(179), SIN8(180), SIN8(181), SIN8(182), SIN8(183),
    SIN8(184), SIN8(185), SIN8(186), SIN8(187), SIN8(188), SIN8(189), SIN8(190), SIN8(191),
    SIN8(192), SIN8(193), SIN8(194), SIN8(195), SIN8(196), SIN8(197), SIN8(198), SIN8(199),
    SIN8(200), SIN8(201), SIN8(202), SIN8(203), SIN8(204), SIN8(205), SIN8(206), SIN8(207),
    SIN8(208), SIN8(209), SIN8(210), SIN8(211), SIN8(212), SIN8(213), SIN8(214), SIN8(215),
    SIN8(216), SIN8(217), SIN8(218), SIN8(219), SIN8(220), SIN8(221), SIN8(222), SIN8(223),
    SIN8(224), SIN8(225), SIN8(226), SIN8(227), SIN8(228), SIN8(229), SIN8(230), SIN8(231),
    SIN8(232), SIN8(233), SIN8(234), SIN8(235), SIN8(236), SIN8(237), SIN8(238), SIN8(239),
    SIN8(240), SIN8(241), SIN8(242), SIN8(243), SIN8(244), SIN8(245), SIN8(246), SIN8(247),
    SIN8(248), SIN8(249), SIN8(250), SIN8(251), SIN8(252), SIN8(253), SIN8(254), SIN8(255),
};
#undef SIN8

uint8_t isin(uint16_t phase16)
{
    /* Top 8 bits = integer index, bottom 8 bits = fractional weight. */
    const uint8_t i0 = (phase16 >> 8) & 0xFF;
    const uint8_t i1 = (uint8_t)(i0 + 1);
    const uint16_t frac = phase16 & 0xFF;       /* 0..255 */
    const uint16_t a = sin_lut8[i0];
    const uint16_t b = sin_lut8[i1];
    return (uint8_t)(a + ((b - a) * frac) / 256);
}

/* 16.16 fixed-point sin table, 1/10th-degree resolution, period 3600.
 *
 * Verbatim from CP-Raycaster-Demo/src/math_tables.h. We keep this so any
 * future raycaster code we port from CP-Raycaster-Demo drops in unchanged.
 *
 * sintable_deg10[i] = round(sin(i / 10 deg) * 65536), for i in [0, 449].
 * i in [360..449] is the tail of the third quadrant + start of fourth,
 * which lets us handle angles slightly past 360 without wrapping. */
#define S(i) ((int32_t)(sinf((i) * (float)M_PI / 1800.0f) * 65536.0f))
const int32_t sintable_deg10[450] = {
    S(0),   S(1),   S(2),   S(3),   S(4),   S(5),   S(6),   S(7),   S(8),   S(9),
    S(10),  S(11),  S(12),  S(13),  S(14),  S(15),  S(16),  S(17),  S(18),  S(19),
    S(20),  S(21),  S(22),  S(23),  S(24),  S(25),  S(26),  S(27),  S(28),  S(29),
    S(30),  S(31),  S(32),  S(33),  S(34),  S(35),  S(36),  S(37),  S(38),  S(39),
    S(40),  S(41),  S(42),  S(43),  S(44),  S(45),  S(46),  S(47),  S(48),  S(49),
    S(50),  S(51),  S(52),  S(53),  S(54),  S(55),  S(56),  S(57),  S(58),  S(59),
    S(60),  S(61),  S(62),  S(63),  S(64),  S(65),  S(66),  S(67),  S(68),  S(69),
    S(70),  S(71),  S(72),  S(73),  S(74),  S(75),  S(76),  S(77),  S(78),  S(79),
    S(80),  S(81),  S(82),  S(83),  S(84),  S(85),  S(86),  S(87),  S(88),  S(89),
    S(90),  S(91),  S(92),  S(93),  S(94),  S(95),  S(96),  S(97),  S(98),  S(99),
    S(100), S(101), S(102), S(103), S(104), S(105), S(106), S(107), S(108), S(109),
    S(110), S(111), S(112), S(113), S(114), S(115), S(116), S(117), S(118), S(119),
    S(120), S(121), S(122), S(123), S(124), S(125), S(126), S(127), S(128), S(129),
    S(130), S(131), S(132), S(133), S(134), S(135), S(136), S(137), S(138), S(139),
    S(140), S(141), S(142), S(143), S(144), S(145), S(146), S(147), S(148), S(149),
    S(150), S(151), S(152), S(153), S(154), S(155), S(156), S(157), S(158), S(159),
    S(160), S(161), S(162), S(163), S(164), S(165), S(166), S(167), S(168), S(169),
    S(170), S(171), S(172), S(173), S(174), S(175), S(176), S(177), S(178), S(179),
    S(180), S(181), S(182), S(183), S(184), S(185), S(186), S(187), S(188), S(189),
    S(190), S(191), S(192), S(193), S(194), S(195), S(196), S(197), S(198), S(199),
    S(200), S(201), S(202), S(203), S(204), S(205), S(206), S(207), S(208), S(209),
    S(210), S(211), S(212), S(213), S(214), S(215), S(216), S(217), S(218), S(219),
    S(220), S(221), S(222), S(223), S(224), S(225), S(226), S(227), S(228), S(229),
    S(230), S(231), S(232), S(233), S(234), S(235), S(236), S(237), S(238), S(239),
    S(240), S(241), S(242), S(243), S(244), S(245), S(246), S(247), S(248), S(249),
    S(250), S(251), S(252), S(253), S(254), S(255), S(256), S(257), S(258), S(259),
    S(260), S(261), S(262), S(263), S(264), S(265), S(266), S(267), S(268), S(269),
    S(270), S(271), S(272), S(273), S(274), S(275), S(276), S(277), S(278), S(279),
    S(280), S(281), S(282), S(283), S(284), S(285), S(286), S(287), S(288), S(289),
    S(290), S(291), S(292), S(293), S(294), S(295), S(296), S(297), S(298), S(299),
    S(300), S(301), S(302), S(303), S(304), S(305), S(306), S(307), S(308), S(309),
    S(310), S(311), S(312), S(313), S(314), S(315), S(316), S(317), S(318), S(319),
    S(320), S(321), S(322), S(323), S(324), S(325), S(326), S(327), S(328), S(329),
    S(330), S(331), S(332), S(333), S(334), S(335), S(336), S(337), S(338), S(339),
    S(340), S(341), S(342), S(343), S(344), S(345), S(346), S(347), S(348), S(349),
    S(350), S(351), S(352), S(353), S(354), S(355), S(356), S(357), S(358), S(359),
    S(360), S(361), S(362), S(363), S(364), S(365), S(366), S(367), S(368), S(369),
    S(370), S(371), S(372), S(373), S(374), S(375), S(376), S(377), S(378), S(379),
    S(380), S(381), S(382), S(383), S(384), S(385), S(386), S(387), S(388), S(389),
    S(390), S(391), S(392), S(393), S(394), S(395), S(396), S(397), S(398), S(399),
    S(400), S(401), S(402), S(403), S(404), S(405), S(406), S(407), S(408), S(409),
    S(410), S(411), S(412), S(413), S(414), S(415), S(416), S(417), S(418), S(419),
    S(420), S(421), S(422), S(423), S(424), S(425), S(426), S(427), S(428), S(429),
    S(430), S(431), S(432), S(433), S(434), S(435), S(436), S(437), S(438), S(439),
    S(440), S(441), S(442), S(443), S(444), S(445), S(446), S(447), S(448), S(449),
};
#undef S
